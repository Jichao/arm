#include "thread.h"
#include "mem/kmalloc.h"

thread_t* _thread_list;

thread_t* create_thread(thread_func_t entry)
{
    thread_t* thread = (thread_t*)kmalloc(sizeof(thread_t));      
    return thread;
}

void start_thread(thread_t* thread)
{

}

void suspend_thread(thread_t* thread)
{

}

void yield(void)
{

}