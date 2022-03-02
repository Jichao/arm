#include "ram.h"
#include "common.h"
#include "clock.h"

#define BWSCON (*(volatile uint32_t*)0x48000000)
#define BANKCON6 (*(volatile uint32_t*)0x4800001C)
#define REFRESH (*(volatile uint32_t*)0x48000024)
#define BANKSIZE (*(volatile uint32_t*)0x48000028)
#define MRSRB6 (*(volatile uint32_t*)0x4800002C)

#define SDRAM_BASE 0x30000000
#define BSS_BASE 0x33c00000
#define BSS_SIZE 1024*100
#define EM63_TRCD_NS 20
#define EM63_TRC_NS 60
#define EM63_TRP_NS 18

void enable_sdram(void)
{
    //not using ub/lb & wait disable & bus width 32bit
    BWSCON &= 0xf0ffffff;
    BWSCON |= 2 << 24;

    //type: sdram & trcd 1 cycle & 9 bit col addr
    /*uint32_t trcd = get_min_hclk_cycle(EM63_TRCD_NS);
    trcd = MAX(trcd, 4);
    trcd = MIN(trcd, 2);
    135MHz 7.4ns per cycle*/
    BANKCON6 = (3 << 15) | (1 << 2) | 1;

    //refresh enable & auto refresh & trp: 3 clock & tsrc: 6 clock & refresh counter: 1009
    REFRESH = (1 << 23) | (1 << 20) | (2 << 18) | 1009;

    //enable brust & enable power down & eanble adaptive sclk & bank 64
    BANKSIZE = 0xb1; //0x1 disable brust & disable power down & disable adaptive sclk

    //cas latency 3 clocks(!!!)
    MRSRB6 = 0x30;
}

/*
void enable_sdram(void)
{
    //not using ub/lb & wait disable & bus width 32bit
    BWSCON &= 0xf0ffffff;
    BWSCON |= 2 << 24;

    //type: sdram & trcd 1 cycle & 9 bit col addr
    uint32_t trcd = get_min_hclk_cycle(EM63_TRCD_NS);
    trcd = MAX(trcd, 4);
    trcd = MIN(trcd, 2);
    BANKCON6 = (3 << 15) | ((trcd - 2) << 2) | 1;

    //refresh enable & auto refresh & trp: 3 clock & tsrc: 6 clock & refresh counter: 996
    uint32_t trp = get_min_hclk_cycle(EM63_TRP_NS);
    trp = MAX(trp, 4);
    trp = MIN(trp, 2);
    uint32_t tsrc = get_min_hclk_cycle(EM63_TRC_NS - EM63_TRP_NS);
    tsrc = MAX(tsrc, 7);
    tsrc = MIN(tsrc, 4);
    uint32_t count = (1 << 11) + 1 - get_hclk() * 7.8;
    REFRESH = (1 << 23) | ((trp - 2) << 20) | ((tsrc - 4) << 18) | count;

    //enable brust & enable power down & eanble adaptive sclk & bank 64
    BANKSIZE = 0xb1; //0x1 disable brust & disable power down & disable adaptive sclk

    //cas latency 3 clocks(!!!)
    MRSRB6 = 0x30;
}*/

//map 0xb0000000 to 0x56000000
void enable_mmu_and_cache(void)
{
    //create section translate table
    for (uint32_t i = 0; i < 4096; ++i) {
        uint32_t sec_base = i * (1<<20);
        if (sec_base == 0xb0000000) {
            sec_base = 0x56000000;
        } 
        uint32_t sec_desc = B16(00001100, 00010010);
        //if in 64m sdram range, enable write-back cache
        if (sec_base >= SDRAM_BASE && sec_base <= (SDRAM_BASE + (64 << 20))) {
            sec_desc = B16(00001100, 00011110);
        }
        (*(uint32_t*)(SDRAM_BASE + i * 4)) =  sec_base + sec_desc;
    }

    //enable icache/dcache/write buffer, disable alignment check
    //c1 = c1 or B16(00010000,00001111);
    __asm__ (
        "ldr r0, =0x30000000\n"
        "mcr p15,0,r0,c2,c0,0\n" //ttb
        "ldr r0, =0x55555555\n"
        "mcr p15,0,r0,c3,c0,0\n" //domain access control register
        "mrc p15,0,r0,c1,c0,0\n"
        "ldr r1, =0x100f\n"
        "orr r0, r1\n"
        "mcr p15,0,r0,c1,c0,0\n" //enable mmu
        :
        :
    );
}

static void bss_zero(void)
{
    for (uint32_t i = BSS_BASE; i < BSS_BASE + BSS_SIZE; i += 4) {
        *((uint32_t*)(i)) = 0;
    }
}

void ram_init(void)
{
    enable_sdram();
    enable_mmu_and_cache();
    bss_zero();
}