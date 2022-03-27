#ifndef _HEAP_HEADER_
#define _HEAP_HEADER_

typedef void* heap_element_t;
typedef int (*compare_func_t)(heap_element_t a, heap_element_t b); 

typedef enum tag_heap_type {
    kMax_Heap,
    kMin_heap,
} heap_type_t;

typedef struct tag_heap {
    int size;
    int alloc_size;
    heap_type_t type;
    heap_element_t* values;
    compare_func_t compare_func;
} heap_t;

heap_t* heap_create(heap_type_t type, int heap_size, 
    compare_func_t compare_func, heap_element_t* values, int values_count);
void heap_append(heap_t* heap, heap_element_t num);
void heap_insert(heap_t* heap, heap_element_t val);
heap_element_t heap_pop(heap_t* heap);
heap_element_t heap_top(heap_t* heap);
void heap_destroy(heap_t* heap);

#endif
