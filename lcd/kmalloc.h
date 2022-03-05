#ifndef _KMALLOC_HEADER_
#define _KMALLOC_HEADER_

#define SECTION_SIZE (1<<20)

#include "bits.h"

void kmalloc_init(void);
void* kmalloc(int size);
void kfree(void* ptr);

#endif