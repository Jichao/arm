#include "entry.h"
#include "led.h"
#include "int.h"
#include "timer.h"
#include "common.h"
#include "uart.h"
#include "stdio.h"
#include "lcd.h"

void on_timer(void)
{
    invert_led(0);
}

void test_lcd(void)
{
    lcd_init();
    lcd_power(0, 1);
    lcd_enable(TRUE);
    clear_screen(0xff00);
}

void entry(void)
{
    led_init();
    interrupt_init();
    set_timer0(2*1000, TRUE, &on_timer);
    uart_init(115200);
    test_lcd();
    while (1) ;
}