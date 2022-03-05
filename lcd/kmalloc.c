#include "common.h"
#include "kmalloc.h"
#include "bits.h"

//refer: https://docs.google.com/document/d/1ERtQnBdY7skZ4Mky2hB40W0jXa9zcjKX0AHaPLB1QEI/edit#
extern char _ram_start;
extern char _bss_end;

#define HEAP_END 0x33ff4800

typedef struct s_meminfo {
    uint32_t heap_start;
    uint32_t heap_end;
    uint8_t* section_bits;
} meminfo_t;

meminfo_t* _meminfo;

static uint32_t round_section(uint32_t addr, BOOL up) {
    if (up) {
        return (addr + SECTION_SIZE - 1) / SECTION_SIZE * SECTION_SIZE;
    } else {
        return addr / SECTION_SIZE * SECTION_SIZE;
    }
}

static int get_section_index(uint32_t addr)
{
    uint32_t off = addr - _meminfo->heap_start;
    return off / SECTION_SIZE;   
}

void kmalloc_init(void)
{
    uint32_t heap_start = round_section((uint32_t)&_bss_end, TRUE);
    uint32_t heap_end = round_section(HEAP_END, FALSE);
    printf("heap start = 0x%x, heap_end = 0x%x\r\n", heap_start, heap_end);
    _meminfo = (meminfo_t*)heap_start;
    _meminfo->heap_start = heap_start;
    _meminfo->heap_end = heap_end;
    _meminfo->section_bits = (uint8_t*)heap_start + offsetof(meminfo_t, section_bits);
    set_bit(_meminfo->section_bits, 0, 1);
}

void* kmalloc(int size)
{
    uint32_t len = round_section(size, TRUE) / SECTION_SIZE;
    int maxlen = (_meminfo->heap_end - _meminfo->heap_start) / SECTION_SIZE;
    if (len > maxlen) {
        return NULL;
    }

    int index = find_empty_bit(_meminfo->section_bits, maxlen, len);
    if (index == -1) {
        return NULL;
    }
    for (int i = index; i < index + len; ++i) {
        set_bit(_meminfo->section_bits, i, 1);
    }
    uint32_t* ptr = (uint32_t*)((char*)_meminfo->heap_start + index * SECTION_SIZE);
    *ptr = len;
    return (void*)(ptr + 1);
}

void kfree(void* ptr)
{
    uint32_t* real_ptr = (uint32_t*)(ptr - 4);
     if ((uint32_t)real_ptr % SECTION_SIZE != 0) {
        printf("section free failed invalid pointer %p\r\n", ptr);
        return;
    }
    int len = *real_ptr;
    int index = get_section_index((uint32_t)real_ptr);
    for (int i = index; i < index + len; ++i) {
        set_bit(_meminfo->section_bits, i, 0);
    }
}