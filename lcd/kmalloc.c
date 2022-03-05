#include "common.h"
#include "kmalloc.h"
#include "bits.h"

//refer: https://docs.google.com/document/d/1ERtQnBdY7skZ4Mky2hB40W0jXa9zcjKX0AHaPLB1QEI/edit#
extern char _ram_start;
extern char _bss_end;

#ifdef TEST
#include <stdlib.h>
#include <assert.h>
#endif

#define HEAP_END 0x33ff4800

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

void zero(char* buffer, int len) {
    for (char* p = buffer; p < buffer + len; ++p) {
        *p = 0;
    }
}

void kmalloc_init(void)
{
#ifdef TEST
    char* ptr = malloc(65 << 20);
    assert(ptr);
    uint32_t heap_start = round_section(ptr, TRUE);
    zero((char*)heap_start, SECTION_SIZE);
    uint32_t heap_end = round_section(heap_start + (64 << 20), FALSE);
#else
    uint32_t heap_start = round_section((uint32_t)&_bss_end, TRUE);
    zero((char*)heap_start, SECTION_SIZE);
    uint32_t heap_end = round_section(HEAP_END, FALSE);
#endif
    printf("heap start = 0x%x, heap_end = 0x%x heap_size = %dM\r\n", 
        heap_start, heap_end, (int)((heap_end - heap_start) / 1024. / 1024.));
    _meminfo = (meminfo_t*)heap_start;
    _meminfo->heap_start = heap_start;
    _meminfo->heap_end = heap_end;
    //_meminfo->section_bits = (uint8_t*)heap_start + offsetof(meminfo_t, section_bits);
    printf("heap_start = %x heap_end = %x section_bits = %x\r\n", _meminfo->heap_start, _meminfo->heap_end, _meminfo->section_bits);
    set_bit(_meminfo->section_bits, 0, 1);
    kassert(is_bit_set(_meminfo->section_bits, 0), "meminfo set failed");
}

void* kmalloc(int size)
{
    uint32_t len = round_section(size, TRUE) / SECTION_SIZE;
    int maxlen = (_meminfo->heap_end - _meminfo->heap_start) / SECTION_SIZE;
    if (len > maxlen) {
        printf("kmalloc allocate error len: %d > max(%d)\n", len, maxlen);
        return NULL;
    }

    int index = find_empty_bit(_meminfo->section_bits, maxlen, len);
    if (index == -1) {
        printf("no empty slot\n");
        return NULL;
    }
    for (int i = index; i < index + len; ++i) {
        set_bit(_meminfo->section_bits, i, 1);
    }
    uint32_t* ptr = (uint32_t*)((char*)_meminfo->heap_start + index * SECTION_SIZE);
    _meminfo->section_len[index] = len;
    printf("index = %d, len= %d ptr = %p\r\n", index, len, ptr);
    return ptr;
}

void kfree(void* ptr)
{
     if ((uint32_t)ptr % SECTION_SIZE != 0) {
        printf("section free failed invalid pointer %p\r\n", ptr);
        return;
    }
    int index = get_section_index((uint32_t)ptr);
    int len = _meminfo->section_len[index];
    for (int i = index; i < index + len; ++i) {
        set_bit(_meminfo->section_bits, i, 0);
    }
}