#include "common.h"
#include "int.h"
#include "led.h"

#define SRCPND (*(volatile uint32_t*)0x4A000000)
#define INTMOD (*(volatile uint32_t*)0x4A000004)
#define INTMSK (*(volatile uint32_t*)0x4A000008)
#define PRIORITY (*(volatile uint32_t*)0x4A00000c)
#define INTPND (*(volatile uint32_t*)0x4A000010)
#define INTOFFSET (*(volatile uint32_t*)0x4A000014)

#define EINTMASK (*(volatile uint32_t*)0x560000a4)
#define EINTPEND (*(volatile uint32_t*)0x560000a8)

#define EINT8_23_OFF 5

void interrupt_init(void)
{
    //gpg-eint map: 0-8 3-11 5-13 6-14
    GPGCONF &= ~(3 | 3 << 6 | 3 << 10 | 3 << 12);
    GPGCONF |= (2 | 2 << 6 | 2 << 10 | 2 << 12);
    EINTMASK &= ~((1 << 8) | (1 << 11) | (1 << 13) | (1 << 14));
    INTMSK &= ~(1 << 5);
    __asm__ (
        "msr cpsr_c, 0x53"
        :
        :
    );
}

static void handle_led(uint32_t int_index, uint32_t led_index)
{
    if (EINTPEND & (1 << int_index)) {
        led_control(led_index);
        EINTPEND &= (1 << int_index);
    }
}

void irq_handler(void)
{
    uint32_t off = INTOFFSET;
    if (off == EINT8_23_OFF) {
        handle_led(8, 0);
        handle_led(11, 0);
        handle_led(13, 0);
        handle_led(14, 0);
    }
    SRCPND = 1<<off;
    INTPND = 1<<off;
}