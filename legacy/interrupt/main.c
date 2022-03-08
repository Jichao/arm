#include "ram.h"
#include "int.h"
#include "led.h"

int main(void)
{
    ram_init();
    led_init();
    interrupt_init();
    while (1) ;
}