#ifndef _CLOCK_HEADER_
#define _CLOCK_HEADER_

#include "common.h"

void clock_init(void);
uint32_t get_fclk(void);
uint32_t get_hclk(void);
uint32_t get_min_hclk_cycle(uint32_t time_ns);
#endif