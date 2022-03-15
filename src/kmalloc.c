#include "common.h"
#include "freelist.h"
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
int _memerr;

static uint32_t round_section(uint32_t addr, BOOL up) {
    return jround(addr, SECTION_SIZE, up);
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
    dprintk("heap start = 0x%x, heap_end = 0x%x heap_size = %dM\r\n", 
        heap_start, heap_end, (int)((heap_end - heap_start) / 1024. / 1024.));
    _meminfo = (meminfo_t*)heap_start;
    _meminfo->heap_start = heap_start;
    _meminfo->heap_end = heap_end;
    dprintk("heap_start = %x heap_end = %x section_bits = %x\r\n", 
        _meminfo->heap_start, _meminfo->heap_end, _meminfo->section_bits);
    set_bit(_meminfo->section_bits, 0, 1);
    kassert(is_bit_set(_meminfo->section_bits, 0), "meminfo set failed");
    _meminfo->free_man.head = freelist_new();
}

void* kmalloc(int size)
{
    if (size == 0) {
        return NULL;
    }
    dprintk("kmalloc size = %d\r\n", size);
    //use bitmap allocation
    if (size >= SECTION_SIZE) {
        uint32_t len = round_section(size, TRUE) / SECTION_SIZE;
        int maxlen = (_meminfo->heap_end - _meminfo->heap_start) / SECTION_SIZE;
        if (len > maxlen) {
            dprintk("kmalloc allocate error len: %d > max(%d)\n", len, maxlen);
            return NULL;
        }

        int index = find_empty_bit(_meminfo->section_bits, maxlen, len);
        if (index == -1) {
            dprintk("no empty slot\n");
            return NULL;
        }
        for (int i = index; i < index + len; ++i) {
            set_bit(_meminfo->section_bits, i, 1);
        }
        uint32_t *ptr =
            (uint32_t *)((char *)_meminfo->heap_start + index * SECTION_SIZE);
        _meminfo->section_len[index] = len;
        dprintk("index = %d, len= %d ptr = %p\r\n", index, len, ptr);
        return ptr;
    } else {
        kassert(_meminfo->free_man.head, "invalid freeman head");
        void* ptr = fmalloc(&_meminfo->free_man, size);
        dprintk("kmalloc ptr = %p\r\n", ptr);
        return ptr;
    }
}

void kfree(void* ptr)
{
    _memerr = 0;
    dprintk("kfree %p\r\n", ptr);
    if ((uint32_t)ptr % SECTION_SIZE == 0) {
        int index = get_section_index((uint32_t)ptr);
        int len = _meminfo->section_len[index];
        for (int i = index; i < index + len; ++i) {
            set_bit(_meminfo->section_bits, i, 0);
        }
    }
    else {
        ffree(&_meminfo->free_man, ptr);
    }
    dprintk("kfree %p done\n", ptr);
}

void * kcalloc(uint32_t items, uint32_t count)
{
    void* ptr = kmalloc(items * count);
    zero(ptr, items * count);
    return ptr;
}

void* krealloc(void* ptr, int new_size)
{
    dprintk("realloc: old-ptr %p, new_size = %d\r\n", ptr, new_size);
    return frealloc(&_meminfo->free_man, ptr, new_size);
}