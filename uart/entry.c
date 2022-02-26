#include "entry.h"
#include "led.h"
#include "int.h"
#include "timer.h"
#include "common.h"
#include "uart.h"

void on_timer(void)
{
    invert_led(0);
}

void entry(void)
{
    led_init();
    interrupt_init();
    set_timer0(2*1000, TRUE, &on_timer);

    uart_init(115200);
    uart_send_string("hello, world echo server started\r\n");
    char buff[50];
    while (1) {
        for (int i = 0; i < 50; ++i) {
            *(buff + i) = 0;
        }
        uart_get_string(buff, 10);
        uart_send_string("\r\nrecv from client:\r\n");
        uart_send_string(buff);
        uart_send_string("\r\n");
    }
}