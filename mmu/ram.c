#include "ram.h"
#include "common.h"

#define BWSCON (*(volatile uint32_t*)0x48000000)
#define BANKCON6 (*(volatile uint32_t*)0x4800001C)
#define REFRESH (*(volatile uint32_t*)0x48000024)
#define BANKSIZE (*(volatile uint32_t*)0x48000028)
#define MRSRB6 (*(volatile uint32_t*)0x4800002C)

#define SDRAM_BASE 0x30000000

void enable_sdram(void)
{
    //not using ub/lb & wait disable & bus width 32bit
    BWSCON &= 0xf0ffffff;
    BWSCON |= 2 << 24;

    //type: sdram & trcd 1 cycle & 9 bit col addr
    BANKCON6 = 0x18001;

    //refresh enable & auto refresh & trp: 0 & tsrc: 0 & refresh counter: 1955
    REFRESH = 0xac07a3;

    //enable brust & enable power down & eanble adaptive sclk & bank 64
    BANKSIZE = 0xb1; //0x1 disable brust & disable power down & disable adaptive sclk

    //cas latency 3 clocks(!!!)
    MRSRB6 = 0x30;
}

//map 0xb0000000 to 0x56000000
void enable_mmu(void)
{
    //create section translate table
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t sec_base = i * (1<<20);
        if (sec_base == 0xb0000000) {
            sec_base = 0x56000000;
        } 
        (*(uint32_t*)(SDRAM_BASE + i * 4)) =  sec_base + B16(00001100, 00010010);
    }

    __asm__ (
        "ldr r0, =0x30000000\n"
        "mcr p15,0,r0,c2,c0,0\n" //ttb
        "ldr r0, =0x55555555\n"
        "mcr p15,0,r0,c3,c0,0\n" //domain access control register
        "mrc p15,0,r0,c1,c0,0\n"
        "orr r0, #0x1\n"
        "mcr p15,0,r0,c1,c0,0\n" //enable mmu
        :
        :
    );
}

void copy_to_sdram(void)
{
    for (uint32_t i = 0; i < 1024 * 4; i+=4) {
        (*(uint32_t*)(SDRAM_BASE + 4096 * 4 + i)) = (*(uint32_t*)(i + 2048));
    }
}

void ram_init(void)
{
    enable_sdram();
    enable_mmu();
    copy_to_sdram();
}