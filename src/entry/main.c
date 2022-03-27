#include "hal/ram.h"
#include "hal/nand.h"
#include "hal/clock.h"
#include "hal/2440addr.h"
#include "entry/entry.h"

typedef void (*t_void_func)(void);

int raise(void)
{
    return 0;
}

void clock_init(void)
{
    __asm__ (
        "mrc p15,0,r0,c1,c0,0\n"
        "orr r0, r0, #0xc0000000\n"
        "mcr p15,0,r0,c1,c0,0\n"
    );
    //superboot
    //fclk:hclk:pclk = 6:2:1
    // 405:135:67.5
    CLKDIV = 0x7; 
    MPLLCON = (0x7f << 12 | 2 << 4 | 1);
}

void disable_alignment_check(void)
{
    __asm__("mrc p15,0,r0,c1,c0,0\n"
            "mvn r1, #2\n"
            "and r0, r0, r1\n"
            "mcr p15,0,r0,c1,c0,0\n" // enable mmu
    );
}

int reset_entry(void)
{
    /*  
    clock_init();
    ram_init();*/

    //如果disable alignment check只是不会data abort而已，但是结果会更严重因为这个时候数据可能是错的。
    //disable_alignment_check();
    bss_zero();
    entry();
    return 0;
}