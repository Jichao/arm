#include "sdram.h"

#define BWSCON (*(volatile unsigned long*)0x48000000)
#define BANKCON6 (*(volatile unsigned long*)0x4800001C)
#define REFRESH (*(volatile unsigned long*)0x48000024)
#define BANKSIZE (*(volatile unsigned long*)0x48000028)
#define MRSRB6 (*(volatile unsigned long*)0x4800002C)
#define SDRAM_BASE 0x30000000

void sdram_init(void)
{
    //not using ub/lb & wait disable & bus width 32bit
    BWSCON &= 0xf0ffffff;
    BWSCON |= 2 << 24;

    //type: sdram & trcd 1 cycle & 9 bit col addr
    BANKCON6 = 0x18001;

    //refresh enable & auto refresh & trp: 0 & tsrc: 0 & refresh counter: 1955
    REFRESH = 0xac07a3;

    //enable brust & enable power down & eanble adaptive sclk & bank 64
    BANKSIZE = 0xb1;

    //cas latency 3 clocks(!!!)
    MRSRB6 = 0x30;

    unsigned long i = 0;
    for (i = 0; i < 4*1024; i+=4) {
        (*(unsigned long*)(SDRAM_BASE + i)) = (*(unsigned long*)(i));
    }
}