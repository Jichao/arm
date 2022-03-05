#ifndef _KMALLOC_HEADER_
#define _KMALLOC_HEADER_

#define SECTION_SIZE (1<<20)

#include "bits.h"
typedef struct s_meminfo {
    uint32_t heap_start;
    uint32_t heap_end;
    uint8_t section_bits[];
} meminfo_t;

extern meminfo_t* _meminfo;

void kmalloc_init(void);
void* kmalloc(int size);
void kfree(void* ptr);

#endif