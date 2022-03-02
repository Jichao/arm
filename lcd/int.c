#include "common.h"
#include "int.h"
#include "led.h"
#include "timer.h"
#include "rtc.h"

#define SRCPND (*(volatile uint32_t*)0x4A000000)
#define INTMOD (*(volatile uint32_t*)0x4A000004)
#define INTMSK (*(volatile uint32_t*)0x4A000008)
#define PRIORITY (*(volatile uint32_t*)0x4A00000c)
#define INTPND (*(volatile uint32_t*)0x4A000010)
#define INTOFFSET (*(volatile uint32_t*)0x4A000014)

#define EINTMASK (*(volatile uint32_t*)0x560000a4)
#define EINTPEND (*(volatile uint32_t*)0x560000a8)

#define EINT8_23_OFF 5
#define INT_TIMER0_OFF 10

#define UNDEF_INSTRUCTION 1
#define SOFTWARE_INTERRUPT 2
#define PREFETCH_ABORT 3
#define DATA_ABORT 4
#define UNUSED_INTERRUPT 5
#define FIQ_INTERRUPT 7

uint32_t _last_sec[24];

void interrupt_init(void)
{
    //gpg-eint map: 0-8 3-11 5-13 6-14
    GPGCONF &= ~(3 | 3 << 6 | 3 << 10 | 3 << 12);
    GPGCONF |= (2 | 2 << 6 | 2 << 10 | 2 << 12);
    EINTMASK &= ~((1 << 8) | (1 << 11) | (1 << 13) | (1 << 14));
    INTMSK &= ~(1 << EINT8_23_OFF) & ~(1 << INT_TIMER0_OFF);
    __asm__ (
        "msr cpsr_c, 0x53"
    );
    memset(_last_sec, 0, sizeof(uint32_t));
}

static void handle_led(uint32_t int_index, uint32_t led_index)
{
    if (EINTPEND & (1 << int_index)) {
        invert_led(led_index);
        EINTPEND &= (1 << int_index);
    }
}

static BOOL is_valid_interval(int index, int interval)
{
    uint32_t curr_count = get_sec_count();
    if (_last_sec[index] == 0) {
        _last_sec[index] = curr_count;
        return TRUE;
    }
    if (curr_count - _last_sec[index] > interval) {
        _last_sec[index] = curr_count;
        return TRUE;
    }
    return TRUE;
}

void irq_handler(void)
{
    uint32_t off = INTOFFSET;
    if (off == EINT8_23_OFF) {
        handle_led(8, 0);
        handle_led(11, 1);
        if (EINTPEND & (1 << 13)) {
            if (is_valid_interval(13, 0)) {
                reset_time();
            }
            EINTPEND &= (1 << 13);
        }
        if (EINTPEND & (1 << 14)) {
            if (is_valid_interval(14, 0)) {
                output_time();
            }
            EINTPEND &= (1 << 14);
        }
        // handle_led(13, 2);
        // handle_led(14, 3);
    } else if (off == INT_TIMER0_OFF) {
        handle_timer0_interrupt();
    }
    SRCPND = 1<<off;
    INTPND = 1<<off;
}

void exception_handler(int index, uint32_t lastpc)
{
    printf("last pc = %0x\r\n", lastpc);
    switch (index) {
    case UNDEF_INSTRUCTION:
        printf("undefined instruction exception\r\n");
        break;
    case PREFETCH_ABORT:
        printf("prefetch abort exception\r\n");
        break;
    case DATA_ABORT:
        printf("data abort exception\r\n");
        break;
    case UNUSED_INTERRUPT:
        printf("unused interrupt\r\n");
        break;
    case SOFTWARE_INTERRUPT:
        printf("software interrupt\r\n");
        break;
    case FIQ_INTERRUPT:
        printf("fiq interrupt\r\n");
        break;
    default:
        printf("unknown interrupt\r\n");
        break;
    }
    printf("unhandled now entering fuck mode\r\n");
    while (1) ; 
}