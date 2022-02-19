#include "ram.h"
#include "led.h"

typedef void (*t_void_func)(void);

int main(void)
{
    ram_init();
    volatile t_void_func ptr = &led_control;
    (*ptr)();
    return 0;
}