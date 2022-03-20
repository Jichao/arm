#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include "base/heap.h"
#include "base/types.h"

typedef void (*timer_callback_t)(void *cb);

typedef enum tag_timer_state {
    kTimer_Uninit,
    kTimer_Started,
    kTimer_Cancelled, // 可以重新start
    kTimer_FiredOnce, // for repeated timer
    kTimer_Destroyed, // 直接删除掉了，无法重新start
    kTimer_Done,
} ktimer_state_t;

typedef struct tag_ktimer {
    uint32_t tick;
    uint32_t interval;
    bool repeat;
    ktimer_state_t state;
    void *cb;
    timer_callback_t callback;
} ktimer_t;

// init 1ms timer0
void init_timer(void);

void handle_timer0_interrupt(uint32_t *pc_ptr);

// get tickcount from startup
uint32_t get_tick_count(void);

// sync delay ns
void delay_ns(uint32_t ns);

// add timer
ktimer_t *timer_create(uint32_t ms, bool repreated, timer_callback_t callback,
                       void *cb, bool start);

int timer_start(ktimer_t *timer);
void timer_stop(ktimer_t *timer);
void timer_destroy(ktimer_t *timer);

extern heap_t *_timer_queue;

#endif