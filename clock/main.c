#include "ram.h"
#include "int.h"
#include "led.h"
#include "clock.h"

int main(void)
{
    clock_init();
    ram_init();
    led_init();
    interrupt_init();
    while (1) ;
}