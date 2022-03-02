#ifndef _INTERRUPT_HEAD_
#define _INTERRUPT_HEAD_

#include "common.h"

void interrupt_init(void);
void irq_handler(void);
void exception_handler(int index, uint32_t lastpc);

#endif