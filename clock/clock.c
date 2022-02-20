#include "common.h"
#include "clock.h"

#define CLKDIV (*(volatile uint32_t*)0x4c000014)
#define MPLLCON (*(volatile uint32_t*)0x4c000004)
#define CAMDIVN (*(volatile uint32_t*)0x4c000018)
#define FIN_FREQ 12

//mini2440
//fclk:最高支持400MHz
//hclk(ahb bus):给soc上的外设用的
//pclk(apb bus):给其他外设用的

void clock_init(void)
{
    __asm__ (
        "mrc p15,0,r0,c1,c0,0\n"
        "orr r0, r0, #0xc0000000\n"
        "mcr p15,0,r0,c1,c0,0\n"
    );
    CLKDIV = 0x7;
    MPLLCON = (0x7f << 12 | 2 << 4 | 1);
}

/*
uint32_t get_fclk(void)
{
    uint32_t sdiv = MPLLCON & 3;
    uint32_t pdiv = (MPLLCON >> 4) & ~(1<<6);
    uint32_t mdiv = (MPLLCON >> 12) & ~(1<<8);
    return (2 * (mdiv + 8) * FIN_FREQ) / ((pdiv + 2) * (1 << sdiv));
}

uint32_t get_hclk(void)
{
    uint32_t fclk = get_fclk();
    uint32_t hdiv = (CLKDIV >> 1) & 3;
    if (hdiv == 0 || hdiv == 1) {
        return fclk / (hdiv + 1);
    }
    if (hdiv == 2) {
        if (CAMDIVN & (1 << 9)) {
            return fclk / 8; 
        } else {
            return fclk / 4;
        }
    } else if (hdiv == 3) {
        if (CAMDIVN & (1 << 9)) {
            return fclk / 6; 
        } else {
            return fclk / 3;
        }
    }
    return fclk;
}

uint32_t get_min_hclk_cycle(uint32_t time_ns) 
{
    return time_ns / (1000 / get_hclk()) + 1;
}
*/