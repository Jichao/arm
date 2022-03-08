#include "sdram.h"

#define GPBCONF (*(volatile unsigned long*)0x56000010)
#define GPBDATA (*(volatile unsigned long*)0x56000014)

#define GPB5_OUT (1<<10)
#define GPB6_OUT (1<<12)
#define GPB7_OUT (1<<14)
#define GPB8_OUT (1<<16)

#define GPB5_MASK (3<<10)
#define GPB6_MASK (3<<12)
#define GPB7_MASK (3<<14)
#define GPB8_MASK (3<<16)

#define GPG0_IN (0xfffc)
#define GPG3_IN (0xff3f)
#define GPG5_IN (0xf3fc)
#define GPG6_IN (0xcfff)

#define GPGCONF (*(volatile unsigned long*)0x56000060)
#define GPGDATA (*(volatile unsigned long*)0x56000064)
#define GPGUP (*(volatile unsigned long*)0x56000068)

void wait(volatile unsigned long dly)
{
	for(; dly > 0; dly--);
}

void light_led1(void)
{
    GPBCONF &= ~GPB5_MASK;
    GPBCONF |= GPB5_OUT;

    GPBDATA &= ~(0x1 << 5);
}

void light_seq(void)
{
    GPBCONF &= ~(GPB5_MASK | GPB6_MASK | GPB7_MASK | GPB8_MASK);
    GPBCONF |= (GPB5_OUT | GPB6_OUT | GPB7_OUT | GPB8_OUT);

    int i = 0;
    while (1) {
        wait(30000);
        GPBDATA |= (0xf<<5);
        GPBDATA &= ~(1<<(i + 5));
        i = (i + 1) % 4;
    }
}

void update_led(int gpg, int gpb)
{
    if ((GPGDATA & (0x1 << gpg)) == 0x0) {
        if (GPBDATA & (1 << gpb)) {
            GPBDATA &= ~(1 << gpb);
        }
        else {
            GPBDATA |= (1 << gpb);
        }
        wait(90000);
    }
}

void led_control(void)
{
    // led1-4
    GPBCONF &= ~(GPB5_MASK | GPB6_MASK | GPB7_MASK | GPB8_MASK);
    GPBCONF |= (GPB5_OUT | GPB6_OUT | GPB7_OUT | GPB8_OUT);

    // k1-4
    GPGCONF = (GPGCONF & GPG0_IN & GPG3_IN & GPG5_IN & GPG6_IN);
    GPGUP = GPGUP & (~(1 << 3)) & (~(1 << 5)) & (~(1 << 6));

    while (1) {
        update_led(0, 5);
        update_led(3, 6);
        update_led(5, 7);
        update_led(6, 8);
    }
}

typedef void (*t_void_func)(void);

int main(void)
{
    sdram_init();
    volatile t_void_func ptr = &led_control;
    (*ptr)();
    return 0;
}