#include "common.h"
#include "clock.h"

//mini2440
//fclk:最高支持400MHz
//hclk(ahb bus):给soc上的外设用的
//pclk(apb bus):给其他外设用的

uint32_t get_fclk(void)
{
    uint32_t pll = MPLLCON;
    uint32_t sdiv = pll & 3;
    uint32_t pdiv = (pll >> 4) & 0x3f;
    uint32_t mdiv = (pll >> 12) & 0xff;
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

uint32_t get_pclk(void)
{
    uint32_t pdiv = CLKDIV & 1;
    if (pdiv) {
        return get_hclk() / 2;
    } else {
        return get_hclk();
    }
}


uint32_t get_min_hclk_cycle(uint32_t time_ns) 
{
    return time_ns / (1000 / get_hclk()) + 1;
}
