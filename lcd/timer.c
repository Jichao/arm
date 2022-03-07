#include "timer.h"
#include "clock.h"

volatile timeout_callback_t timer0_callback = 0;

void handle_timer0_interrupt()
{
    if (timer0_callback)
        timer0_callback();
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
        TCON &= ~1 & ~(1<<8) & ~(1<<12) & ~(1<<20);
    }
}

void set_timer0(uint32_t ms, int repeat, timeout_callback_t callback)
{
    if (callback == 0) {
        return;
    }
    timer0_callback = callback; 

    //minimum interval:
    //  pclk/{prescaler+1}/divider
    //  (400/3./2.)/256/16 = 0.016MHz
    //  1/0.016=61.44us
    //for timer0
    TCFG0 = TCFG0 | 0xff; //prescaler 255
    TCFG1 = (TCFG1 & ~0xf) | 0x3;    //divider 16

    //set the count
    TCNTB0 = ms * 1000 * (get_pclk() / 256. / 16);
    TCMPB0 = 0;

    //manual load count
    TCON |= (1<<1);
    //auto reload (repeated) | start for timer0
    TCON = 0x9;   
}

//delay all use timer2
void delay_ns(uint32_t ns)
{
    //timer2-4 prescaler 1
    TCFG0 = (TCFG0 & ~(0xff << 8)) | (0 << 8);
    //divider 1
    TCFG1 = (TCFG1 & ~(0xf << 8)) | (0x4 << 8);
    //interval = (400/3./2.)/1 = 67.5MHz
    //  1/67.5 = 14ns

    int min_unit = 1000 / get_pclk();
    if (ns < min_unit) {
        ns = min_unit;
    }
    int count = ns / min_unit;
    printf("delay %u ns unit: %d ns count = %d\r\n", ns, min_unit, count);

    TCNTB2 = count;
    TCMPB2 = 0;

    //manual load count
    TCON |= (1<<13);

    //one short && start for timer2
    TCON = (TCON & ~(0xf << 12)) | (0x1 << 12);
    while (TCNTO2) {
        printf("count = %u\r\n", TCNTO2);
    };

    //stop timer2?
    TCON = TCON & ~(0x1 << 12);
}