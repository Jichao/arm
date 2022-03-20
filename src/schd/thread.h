#ifndef _THREAD_HEADER_
#define _THREAD_HEADER_

#include "base/types.h"
#include "base/list.h"

typedef enum tag_thread_state {
    kThread_New,
    kThread_Running,
    kThread_Waiting, //等待事件（比如讲spinlock)
    kThread_Suspend, //主动操作的挂起
    kThread_Done,
} thread_state_t;

typedef struct tag_cpu_context {
    uint32_t regs[13]; 
    void* sp; //r13
    void* lr; //r14
    void* pc; //r15
} cpu_context_t;

typedef int (*thread_func_t)(void* cb);

typedef struct tag_thread {
    dlist_entry_t list;
    uint32_t tid;
    thread_func_t entry;
    thread_state_t state;
    void* stack;
    uint32_t stack_size;
    cpu_context_t context;
    char name[8];
    int ret;
} thread_t;

thread_t* thread_create(thread_func_t entry);
void thread_start(thread_t* thread);
void thread_destroy(thread_t* thread);
void thread_set_name(thread_t* thread, const char* name);
void dump_thread(thread_t* thread);

#endif