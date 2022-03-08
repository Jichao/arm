#ifndef _LED_H_
#define _LED_H_

#define GPBCONF (*(volatile uint32_t*)0xb0000010)
#define GPBDATA (*(volatile uint32_t*)0xb0000014)
#define GPGCONF (*(volatile uint32_t*)0xb0000060)
#define GPGDATA (*(volatile uint32_t*)0xb0000064)
#define GPGUP (*(volatile uint32_t*)0xb0000068)

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

void led_init(void);
void led_control(int index);

#endif