#include "ram.h"
#include "nand.h"
#include "clock.h"
#include "entry.h"

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
    //fclk:hclk:pclk = 6:2:1
    // 405:135:67.5
    CLKDIV = 0x7; 
    MPLLCON = (0x7f << 12 | 2 << 4 | 1);
}

int main(void)
{
    /*  
    clock_init();
    ram_init();*/
    bss_zero();
    entry();
    return 0;
}