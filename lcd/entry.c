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

    lcd_enable(FALSE);
    lcd_power(0, 0);
}

void entry(void)
{
    led_init();
    uart_init(115200);
    printf("\r\nentry good\r\n");
    interrupt_init();
    set_timer0(2*1000, TRUE, &on_timer);
    test_lcd();
    while (1) ;
}