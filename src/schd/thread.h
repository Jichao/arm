#ifndef _THREAD_HEADER_
#define _THREAD_HEADER_

#include "base/types.h"
#include "base/list.h"

typedef enum tag_thread_status {
    kThread_New,
    kThread_Running,
    kThread_Waiting, //等待事件（比如讲spinlock)
    kThread_Suspend, //主动操作的挂起
    kThread_Done,
} thread_status_t;

typedef int (*thread_func_t)(void);
typedef struct tag_thread {
    dlist_entry_t next;
    uint32_t tid;
    thread_func_t entry;
    thread_status_t status;
} thread_t;

thread_t* create_thread(thread_func_t entry);
void suspend_thread(thread_t* thread);
void start_thread(thread_t* thread);
void yield(void);

#endif