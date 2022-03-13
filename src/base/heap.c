#include "heap.h"
#include "../common.h"
#include "../kmalloc.h"

static void swap(heap_element_t values[], int i, int j)
{
    if (i == j) {
        return;
    }
    heap_element_t t = values[i];
    values[i] = values[j];
    values[j] = t;
}

static int left_child_index(int index) { return index * 2 + 2; }

static int right_child_index(int index) { return index * 2 + 1; }

static int parent_index(int index)
{
    if (index == 0) {
        return index;
    }
    else {
        return (index - 1) / 2;
    }
}

// equal 0
// a > b 1
// a < b -1
static int compare_element(heap_t *heap, heap_element_t a, heap_element_t b)
{
    if (heap->compare_func) {
        return heap->compare_func(a, b);
    }
    else {
        if (a == b) {
            return 0;
        }
        else if (a < b) {
            return -1;
        }
        else {
            return 1;
        }
    }
}

static BOOL need_swap(heap_t *heap, int i, int j)
{
    if (heap->values[i] == heap->values[j]) {
        return FALSE;
    }
    int res = compare_element(heap, heap->values[i], heap->values[j]);
    if (heap->type == kMin_heap) {
        return res > 0;
    }
    else {
        return res < 0;
    }
}

static void shift_up(heap_t *heap, int index)
{
    if (index == 0) {
        return;
    }
    int parent = parent_index(index);
    if (need_swap(heap, parent, index)) {
        swap(heap->values, parent, index);
        shift_up(heap, parent);
    }
    return;
}

static void shift_down(heap_t *heap, int index)
{
    int ti = index;
    int l = left_child_index(index);
    int r = right_child_index(index);
    if (l < heap->size && need_swap(heap, index, l)) {
        ti = l;
    }
    if (r < heap->size && need_swap(heap, ti, r)) {
        ti = r;
    }
    if (ti != index) {
        swap(heap->values, ti, index);
        shift_down(heap, ti);
    }
}

void heap_insert(heap_t *heap, heap_element_t val)
{
    if ((heap->type == kMin_heap &&
         compare_element(heap, val, heap->values[0]) > 0) ||
        (heap->type == kMax_Heap &&
         compare_element(heap, val, heap->values[0]) < 0)) {
        heap->values[0] = val;
        shift_down(heap, 0);
    }
}
heap_t *heap_create(heap_type_t type, int heap_size,
                    compare_func_t compare_func, heap_element_t *values,
                    int values_count)
{
    if (heap_size == 0) {
        return 0;
    }

    heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));
    heap->alloc_size = heap_size;
    heap->size = heap_size;
    heap->type = type;
    heap->compare_func = compare_func;
    heap->values = 0;

    if (heap_size) {
        heap->values =
            (heap_element_t *)kmalloc(heap_size * sizeof(heap_element_t));
        int size = MIN(heap_size, values_count);
        for (int i = 0; i < size; ++i) {
            heap->values[i] = values[i];
        }
        for (int i = parent_index(heap_size - 1); i >= 0; i--) {
            shift_down(heap, i);
        }
        for (int i = heap_size; i < values_count; ++i) {
            heap_insert(heap, values[i]);
        }
    }
    return heap;
}

heap_element_t heap_pop(heap_t *heap)
{
    heap_element_t v = heap->values[0];
    swap(heap->values, 0, heap->size - 1);
    heap->size--;
    shift_down(heap, 0);
    return v;
}

void heap_append(heap_t *heap, heap_element_t num)
{
    if (heap->size + 1 > heap->alloc_size) {
        int new_alloc_size = heap->size * 2;
        heap->values = (heap_element_t *)krealloc(
            heap->values, new_alloc_size * sizeof(heap_element_t));
    }
    heap->values[heap->size++] = num;
    shift_up(heap, heap->size - 1);
}

void heap_destroy(heap_t *heap)
{
    kfree(heap->values);
    kfree(heap);
}

heap_element_t heap_top(heap_t *heap)
{
    if (heap->size > 0) {
        return heap->values[0];
    }
    else {
        return NULL;
    }
}
