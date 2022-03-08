#include "jump.h"
#include "led.h"
#include "int.h"
#include "timer.h"
#include "common.h"

void on_timer(void)
{
    invert_led(0);
}

void jump(void)
{
    led_init();
    interrupt_init();
    set_timer0(2*1000, TRUE, &on_timer);
    while (1) ; 
}