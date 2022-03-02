#include "timer.h"
#include "clock.h"

#define TCFG0 (*(volatile uint32_t*)0x51000000)
#define TCFG1 (*(volatile uint32_t*)0x51000004)
#define TCON (*(volatile uint32_t*)0x51000008)
#define TCNTB0 (*(volatile uint32_t*)0x5100000C)
#define TCMPB0 (*(volatile uint32_t*)0x51000010)

volatile timeout_callback_t timer0_callback;

void handle_timer0_interrupt()
{
    timer0_callback();
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
    TCFG0 = 0xff; //prescaler 255
    TCFG1 = 3;    //divider 16
    TCNTB0 = ms * 1000 * (get_pclk() / 256. / 16);
    TCMPB0 = 0;
    TCON |= (1<<1);
    TCON = 0x9;   
}