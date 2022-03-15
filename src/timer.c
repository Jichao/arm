#include "timer.h"
#include "base/heap.h"
#include "clock.h"
#include "kmalloc.h"

volatile uint32_t _tick = 0;

heap_t *_timer_queue;

int compare_timer(heap_element_t a, heap_element_t b)
{
    ktimer_t *ta = (ktimer_t *)a;
    ktimer_t *tb = (ktimer_t *)b;
    if (ta->tick == tb->tick) {
        return 0;
    }
    else if (ta->tick > tb->tick) {
        return 1;
    }
    else {
        return -1;
    }
}

void init_timer(void)
{
    // minimum interval:
    //   pclk/{prescaler+1}/divider
    //   (400/3./2.)/256/16 = 0.016MHz
    //   1/0.016=61.44us
    // for timer0
    TCFG0 = TCFG0 | 0xff;         // prescaler 255
    TCFG1 = (TCFG1 & ~0xf) | 0x3; // divider 16

    // set the count
    // 1ms one tick
    TCNTB0 = 1 * 1000 * (get_pclk() / 256. / 16);
    TCMPB0 = 0;

    // manual load count
    TCON |= (1 << 1);
    // auto reload (repeated) | start for timer0
    TCON = 0x9;

    _timer_queue = heap_create(kMin_heap, 0, &compare_timer, NULL, 0);
}

void dispatch_timer(void)
{
    ktimer_t *timer;
    while (TRUE) {
        timer = (ktimer_t *)heap_top(_timer_queue);
        if (!timer) {
            // dprintk("no timer in the timer queue\r\n");
            break;
        }

        if (timer->state == kTimer_Cancelled) {
            // dprintk("timer cancelled\r\n");
            heap_pop(_timer_queue);
        }
        if (timer->state == kTimer_Destroyed) {
            heap_pop(_timer_queue);
            kfree(timer);
        }
        else {
            if (_tick >= timer->tick) {
                // dprintk("timer fired\r\n");
                heap_pop(_timer_queue);
                if (timer->repeat) {
                    timer->tick += timer->interval;
                    timer->state = kTimer_FiredOnce;
                    heap_append(_timer_queue, timer);
                }
                else {
                    timer->state = kTimer_Done;
                }
                if (timer->callback) {
                    timer->callback(timer->cb);
                }
            }
            else {
                // dprintk("top timer tick = %u curr tick = %u\r\n",
                // timer->tick, _tick);
                break;
            }
        }
    };
}

void handle_timer0_interrupt()
{
    _tick++;
    dispatch_timer();
}

void disable_timer(int index)
{
    if (index == 0) {
        TCON &= ~1;
    }
    if (index == 2) {
        TCON &= ~(1 << 8);
    }
    if (index == 3) {
        TCON &= ~(1 << 12);
    }
    if (index == 4) {
        TCON &= ~(1 << 20);
    }
    if (index == -1) {
        TCON &= ~1 & ~(1 << 8) & ~(1 << 12) & ~(1 << 20);
    }
}

// delay all use timer2
void delay_ns(uint32_t ns)
{
    // timer2-4 prescaler 1
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (0 << 8);
    // divider 1
    TCFG1 = (TCFG1 & ~(0xf << 8)) | (0x4 << 8);
    // interval = (400/3./2.)/1 = 67.5MHz
    //   1/67.5 = 14ns

    int min_unit = 1000 / get_pclk();
    if (ns < min_unit) {
        ns = min_unit;
    }
    int count = ns / min_unit;
    // printf("delay %u ns unit: %d ns count = %d\r\n", ns, min_unit, count);

    TCNTB2 = count;
    TCMPB2 = 0;

    // manual load count
    TCON |= (1 << 13);

    // one short && start for timer2
    TCON = (TCON & ~(0xf << 12)) | (0x1 << 12);
    while (TCNTO2) {
        // printf("count = %u\r\n", TCNTO2);
    };

    // stop timer2?
    TCON = TCON & ~(0x1 << 12);
}

uint32_t get_tick_count(void) { return _tick; }

ktimer_t *create_timer(uint32_t ms, BOOL repreated, timer_callback_t callback,
                       void *cb, bool start)
{
    dprintk("create timer: %u ms repreat: %d, start: %d\r\n", ms, repreated,
            start);
    ktimer_t *timer = (ktimer_t *)kmalloc(sizeof(ktimer_t));
    timer->callback = callback;
    timer->repeat = repreated;
    timer->interval = ms;
    timer->state = kTimer_Uninit;
    timer->cb = cb;

    if (start) {
        start_timer(timer);
    }
    return timer;
}

int start_timer(ktimer_t *timer)
{
    dprintk("start timer: %p\r\n", timer);
    if (timer->state == kTimer_Uninit) {
        timer->tick = _tick + timer->interval;
        dprintk("heap append timer %p\r\n", timer);
        heap_append(_timer_queue, timer);
        return 0;
    }
    return -1;
}

void stop_timer(ktimer_t *timer) { timer->state = kTimer_Cancelled; }

void destroy_timer(ktimer_t *timer) { timer->state = kTimer_Destroyed; }
