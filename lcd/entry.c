#include "entry.h"
#include "led.h"
#include "int.h"
#include "timer.h"
#include "common.h"
#include "uart.h"
#include "stdio.h"
#include "lcd.h"
#include "girl.h"

void on_timer(void)
{
    invert_led(0);
}

void test_lcd(void)
{
    printf("start test lcd\r\n");

    lcd_init();
    lcd_power(0, 1);
    lcd_enable(TRUE);

    printf("clear lcd with red\r\n");
    clear_screen((0b11111 << 11));
    getc();

    printf("draw rect\r\n");
    test_rect();
    getc();

    printf("draw girl\r\n");
    draw_24bmp(girl_180x320, 180, 320, 30, 0);
    getc();

    printf("lcd test done turning off\r\n");
    lcd_enable(FALSE);
    lcd_power(0, 0);
}

void entry(void)
{
    led_init();
    printf("before interrupt inited\r\n");
    interrupt_init();
    printf("interupt inited\r\n");
    set_timer0(2*1000, TRUE, &on_timer);
    printf("timer0 inited\r\n");
    while (1) {
        test_lcd();
    }
}