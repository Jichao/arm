//test program for kmalloc
#include "bits.h"
#include "kmalloc.h"
#include <assert.h>
#include <time.h> 
#include <stdlib.h>
#include "apple_wav.h"
#include "wav.h"

static void* get_ptr(int index) {
    return (void*)(_meminfo->heap_start + (index + 4) * SECTION_SIZE);
}

static int get_size(int start, int limit) {
    return start + rand()  % limit;
}

void test_cont_malloc()
{
    //malloc malloc free malloc free malloc 
    void *ptr = kmalloc(get_size(1 << 20, 5000));
    printf("ptr = %p, heap_start = %p\n", ptr, (void*)_meminfo->heap_start);
    assert(ptr == get_ptr(1));

    void* ptr2 = kmalloc(get_size(1 << 20, -5000));    
    assert(ptr2 == get_ptr(3));
    kfree(ptr2);

    ptr2 = kmalloc(get_size(1 << 20, -5000));    
    assert(ptr2 == get_ptr(2));
    kfree(ptr2);

    ptr2 = kmalloc(get_size(1 << 20, -5000));    
    assert(ptr2 == get_ptr(2));
    kfree(ptr2);
    kfree(ptr);

    ptr = kmalloc((3 << 20) + 1);
    assert(ptr == get_ptr(1));

    ptr2 = kmalloc((2 << 20) + 1);
    assert(ptr2 == get_ptr(5));

    void* ptr3 = kmalloc((1 << 20) + 1);
    assert(ptr3 == get_ptr(6));

    kfree(ptr2);
    ptr2 = kmalloc((1 << 20) + 1);
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

void test_cont_malloc2(void)
{
    //cont and free
    printf("start testing malloc2\n\n");
    void* ptr0 = kmalloc(32);
    void* ptr1 = kmalloc(41);
    void* ptr2 = kmalloc(41);

    assert(ptr0 != 0 && ptr1 != 0 && ptr2 != 0);
    printf("ptr0 = %p, ptr1 = %p, ptr2 = %p\n", ptr0, ptr1, ptr2);
    kfree(ptr0);

    void* ptr3 = kmalloc(32);
    printf("ptr3 = %p\n", ptr3);
    assert(ptr0 == ptr3);

    kfree(ptr2);
    void* ptr4 = kmalloc(42);
    assert(ptr4 == ptr2);
}

void test_for_loop_malloc()
{
    for (int i = 0; i < 10; ++i) {
        char* ptr = kmalloc(320*180*2);
        kfree(ptr);
        assert(_memerr == 0);
    }
}

void test_read_wav_file()
{
    wav_format_t *wav = read_wav_file(apple_wav_file, 1764098);
    assert(wav);
}

int main(void)
{
    printf("start initing...\n");
    kmalloc_init();
    printf("start testing...\n");
    srand((unsigned)time(NULL)); 
    test_read_wav_file();
    //test_cont_malloc();
    // test_bits();
    // test_cont_malloc2();
    // test_for_loop_malloc();
    printf("end testing...\n");
    return 0;
}