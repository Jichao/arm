#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include "common.h"

typedef void(*timeout_callback_t)(void);

void handle_timer0_interrupt(void);
void set_timer0(uint32_t ms, int repeat, timeout_callback_t callback);
void disable_timer(int index);
void delay_ns(uint32_t ns);
uint32_t get_tick_count(void);
// void delay_ms(uint32_t ms);

#endif