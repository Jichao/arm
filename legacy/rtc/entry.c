#include "entry.h"
#include "led.h"
#include "int.h"
#include "timer.h"
#include "common.h"
#include "uart.h"
#include "stdio.h"

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
    printf("\n===begin echo server===\n");

    char buff[50];
    while (1) {
        for (int i = 0; i < 50; ++i) {
            *(buff + i) = 0;
        }
        uart_get_string(buff, 10);
        printf("\n\rrecv from client:%s\r\n", buff);
    }
}