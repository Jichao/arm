#include "led.h"
#include "common.h"

static void wait(volatile uint32_t dly)
{
	for(; dly > 0; dly--);
}

static void update_led(int gpb)
{
    if (GPBDATA & (1 << gpb)) {
        GPBDATA &= ~(1 << gpb);
    }
    else {
        GPBDATA |= (1 << gpb);
    }
}

void led_init(void) 
{
    //gpb 5,6,7,8对应开头k1,k2,k3,k4
    //设置gpb 5,6,7,8为output pin
    //并且设置成低电压
    GPBCONF &= ~(GPB5_MASK | GPB6_MASK | GPB7_MASK | GPB8_MASK);
    GPBCONF |= (GPB5_OUT | GPB6_OUT | GPB7_OUT | GPB8_OUT);
    GPBDATA &= ~((1<<5) | (1<<6) | (1<<7) | (1<<8));
}

void invert_led(int index)
{
    switch (index) {
    case 0:
        update_led(5);
        break;
    case 1:
        update_led(6);
        break;
    case 2:
        update_led(7);
        break;
    case 3:
        update_led(8);
        break;
    default:
        break;
    }
}
