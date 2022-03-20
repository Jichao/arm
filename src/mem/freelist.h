#ifndef _FREELIST_HEADER_
#define _FREELIST_HEADER_

#include "base/common.h"

typedef struct s_freelist {
    struct s_freelist* next;
    uint32_t len;
} freelist_entry_t;

typedef struct s_freelist_manager {
    freelist_entry_t* head;
    uint32_t size;      //all memory(include the metadata)
    uint32_t full_size; //full memory(include the metadata)
    uint32_t frag;      //fragmentataion
    uint32_t entry_count; //entry count
} freelist_man_t;

freelist_entry_t *freelist_new(void);
void* fmalloc(freelist_man_t* free_man, int size);
void ffree(freelist_man_t* free_man, void* ptr);
void* frealloc(freelist_man_t* free_man, void* ptr, int new_size);

#endif