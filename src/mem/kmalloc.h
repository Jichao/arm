#ifndef _KMALLOC_HEADER_
#define _KMALLOC_HEADER_

#define SECTION_SIZE (1 << 20)

#include "base/bits.h"
#include "base/types.h"
#include "freelist.h"

typedef struct s_meminfo {
    uint32_t heap_start;
    uint32_t heap_end;
    freelist_man_t free_man;
    uint8_t section_bits[16]; // support 128M
    uint8_t section_len[];
} meminfo_t;

extern meminfo_t *_meminfo;
extern int _memerr;

void kmalloc_init(void);
void *kmalloc(int size);
void *kzmalloc(int size);
void *kcalloc(uint32_t items, uint32_t count);
void kfree(void *ptr);
void *krealloc(void *ptr, int new_size);

#endif