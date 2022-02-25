#ifndef _CLOCK_HEADER_
#define _CLOCK_HEADER_

#include "common.h"

#define CLKDIV (*(volatile uint32_t*)0x4c000014)
#define MPLLCON (*(volatile uint32_t*)0x4c000004)
#define CAMDIVN (*(volatile uint32_t*)0x4c000018)
#define FIN_FREQ 12

void clock_init(void);
uint32_t get_fclk(void);
uint32_t get_hclk(void);
uint32_t get_pclk(void);
uint32_t get_min_hclk_cycle(uint32_t time_ns);

#endif