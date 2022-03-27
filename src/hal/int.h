#ifndef _INTERRUPT_HEAD_
#define _INTERRUPT_HEAD_

#include "base/types.h"
#include "schd/thread.h"

extern cpu_context_t _irq_context;

typedef void (*switch_func_t)(void* cb);

void interrupt_init(void);
void irq_handler(void* context);
void exception_handler(int index, uint32_t lastpc);
void set_switch_callback(int switch_index, switch_func_t func, void* cb);

void disable_irq(void);
void restore_irq(void);

#endif