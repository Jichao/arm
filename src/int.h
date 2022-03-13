#ifndef _INTERRUPT_HEAD_
#define _INTERRUPT_HEAD_

#include "common.h"

typedef void (*switch_func_t)(void* cb);

void interrupt_init(void);
void irq_handler(void);
void exception_handler(int index, uint32_t lastpc);
void set_switch_callback(int switch_index, switch_func_t func, void* cb);
#endif