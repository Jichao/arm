#include "schd/schd.h"
#include "base/list.h"
#include "base/macros.h"
#include "base/utils.h"
#include "schd/spinlock.h"
#include "schd/thread.h"
#include "hal/int.h"
#include "thread.h"
#include <stdio.h>
#include <string.h>

schd_t _schd;

#define TIME_SLICE 600

int schd_run(void *cb);

/*scheduler
//scheduler timer(irq)
//target:  schedule(save current thread state and switch to next thread)
0) save current thread state on tcb (r0-r12,pc) without(lr,sp)
1) change pc to schd_switch.
2) schd_run get current lr,sp save to current thread state(tcb).
3) get next_thread.
4) switch to next_thread set all registers with thread state.
*/

int idle_thread(void *cb)
{
    while (true)
        ;
    return 0;
}

thread_t *schd_init(thread_func_t entry)
{
    thread_t *thread = thread_create(entry);
    _schd.threads = dlist_create(&thread->list);
    _schd.thread_count = 1;
    _schd.curr_thread = thread;
    _schd.timer = timer_create(TIME_SLICE, true, nullptr, nullptr, false);
    return thread;
}

void schd_add_thread(thread_t *thread)
{
    dlist_append(_schd.threads, &thread->list);
    _schd.thread_count++;
}

void schd_start(void) { timer_start(_schd.timer); }

static void* switch_to_next(void)
{
    memcpy(&_schd.curr_thread->context, &_irq_context, sizeof(cpu_context_t));
    printk("===before switch===\r\n");
    schd_dump();
    thread_t* thread = (thread_t*)_schd.curr_thread->list.next;
    if (!thread) {
        thread = (thread_t*)_schd.threads->head;
    }
    _schd.curr_thread = thread;
    return &thread->context;
}

void* schd_switch_imp(void* sp, void* lr)
{
    _irq_context.sp = sp;
    _irq_context.lr = lr;
    return switch_to_next();
}

void thread_suspend(thread_t *thread) { thread->state = kThread_Suspend; }

void thread_yield(void) {}

void schd_dump(void)
{
    printk("thread count = %d\r\n", _schd.thread_count);
    printk("curr thread = %s\r\n", _schd.curr_thread->name);
    printk("next thread = %s\r\n", ((thread_t*)_schd.curr_thread->list.next)->name);
    dlist_entry_t* entry = _schd.threads->head;
    for (; entry!= nullptr; entry = entry->next)
    {
        dump_thread((thread_t*)entry);
    }
}
