#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include "common.h"
#include "base/heap.h"

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
    BOOL repeat;
    ktimer_state_t state;
    void *cb;
    timer_callback_t callback;
} ktimer_t;

// init 1ms timer0
void init_timer(void);

// irq handler
void handle_timer0_interrupt(void);

// get tickcount from startup
uint32_t get_tick_count(void);

// sync delay ns
void delay_ns(uint32_t ns);

// add timer
ktimer_t *create_timer(uint32_t ms, BOOL repreated, timer_callback_t callback,
                    void *cb, BOOL start);

int start_timer(ktimer_t *timer);
void stop_timer(ktimer_t *timer);
void destroy_timer(ktimer_t *timer);

extern heap_t *_timer_queue;

#endif