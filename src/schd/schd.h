#ifndef _SCHEDULER_HEADER_
#define _SCHEDULER_HEADER_

#include "base/list.h"
#include "schd/spinlock.h"
#include "schd/thread.h"
#include "schd/timer.h"

typedef struct tag_schd {
    spinlock_t lock;
    dlist_t* threads;
    thread_t* curr_thread;
    int thread_count;
    ktimer_t* timer;
}schd_t;

extern schd_t _schd;

thread_t* schd_init(thread_func_t entry);
void schd_start(void);
void schd_switch(void);

void schd_add_thread(thread_t* thread);
void schd_suspend_thread(thread_t* thread);
void schd_yield(void);

#endif