//test program for kmalloc
#include "bits.h"
#include "kmalloc.h"
#include <assert.h>

static void* get_ptr(int index) {
    return (void*)(_meminfo->heap_start + index * SECTION_SIZE + 4);
}

void test_cont_malloc()
{
    //malloc malloc free malloc free malloc 
    void *ptr = kmalloc(320 * 180 * 2);
    printf("ptr = %p, heap_start = %p\n", ptr, (void*)_meminfo->heap_start);
    assert(ptr == get_ptr(1));

    void* ptr2 = kmalloc(320 * 180 * 2);
    assert(ptr2 == get_ptr(2));
    kfree(ptr2);

    ptr2 = kmalloc(320 * 180 * 2);
    assert(ptr2 == get_ptr(2));
    kfree(ptr2);

    ptr2 = kmalloc(320 * 180 * 2);
    assert(ptr2 == get_ptr(2));
    kfree(ptr2);
    kfree(ptr);

    ptr = kmalloc((3 << 20) + 1);
    assert(ptr == get_ptr(1));

    ptr2 = kmalloc(1);
    assert(ptr2 == get_ptr(5));

    void* ptr3 = kmalloc(1);
    assert(ptr3 == get_ptr(6));

    kfree(ptr2);
    ptr2 = kmalloc(1);
    assert(ptr2 == get_ptr(5));

    void* full_ptr = kmalloc((64 - 6) << 20);
    assert(full_ptr == NULL);
}

void test_bits()
{
    uint8_t buff[8] = {0};
    set_bit(buff, 0, 1);
    assert(is_bit_set(buff, 0) == TRUE);
    int index = find_empty_bit(buff, 16, 1);
    assert(index == 1);
}

int main(void)
{
    printf("start testing...\n");
    kmalloc_init();
    test_bits();
    test_cont_malloc();
    printf("end testing...\n");
    return 0;
}