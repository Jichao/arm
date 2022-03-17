#include "hal/2440addr.h"
#include "hal/led.h"
#include "hal/rtc.h"
#include "base/macros.h"
#include "hal/int.h"
#include "timer.h"

extern uint32_t _irq;

#define EINT8_23_OFF 5
#define INT_TIMER0_OFF 10

#define UNDEF_INSTRUCTION 1
#define SOFTWARE_INTERRUPT 2
#define PREFETCH_ABORT 3
#define DATA_ABORT 4
#define UNUSED_INTERRUPT 5
#define FIQ_INTERRUPT 7

#define IRQ_HANDLER_ADDR (*(volatile uint32_t*)0x33f80440)

typedef struct tag_switch_conf {
    int eint_index;
    int last_tick;
    switch_func_t callback;
    void* cb;
} switch_conf_t;

//8:11:13:14:15:19
switch_conf_t _switch_confs[6] = {
    {8,0,0,0},
    {11,0,0,0},
    {13,0,0,0},
    {14,0,0,0},
    {15,0,0,0},
    {19,0,0,0},
};

void interrupt_init(void)
{
    //gpg-eint map: 0-8 3-11 5-13 6-14
    GPGCONF &= ~(3 | 3 << 6 | 3 << 10 | 3 << 12);
    GPGCONF |= (2 | 2 << 6 | 2 << 10 | 2 << 12);
    __asm__ (
        "msr cpsr_c, 0xd3"
    );

    EINTMASK = 0x000fffff;
    INTMSK = 0xffffffff;

    IRQ_HANDLER_ADDR = 0xe59ff000; //ldr pc, [pc, #4]
    *(&IRQ_HANDLER_ADDR + 1) = (uint32_t)&_irq;
    *(&IRQ_HANDLER_ADDR + 2) = (uint32_t)&_irq;
    EINTMASK &= ~((1 << 8) | (1 << 11) | (1 << 13) | (1 << 14));
    INTMSK &= ~(1 << EINT8_23_OFF) & ~(1 << INT_TIMER0_OFF);
    __asm__ (
        "msr cpsr_c, 0x53"
    );
}

/*static void handle_led(uint32_t int_index, uint32_t led_index)
{
    if (EINTPEND & (1 << int_index)) {
        invert_led(led_index);
        EINTPEND &= (1 << int_index);
    }
}*/

void switch_handler(void)
{
    for (int i = 0; i < jcountof(_switch_confs); ++i) {
        switch_conf_t* conf = &_switch_confs[i];
        if (EINTPEND & (1 << conf->eint_index)) {
            uint32_t curr_tick = get_tick_count();
            if (conf->last_tick == 0 || curr_tick - conf->last_tick > 1000) {
                conf->last_tick= curr_tick;
                if (conf->callback) {
                    conf->callback(conf->cb);
                }
            }
            EINTPEND &= (1 << conf->eint_index);
        }
    }
}

void irq_handler(void)
{
    uint32_t off = INTOFFSET;

    if (off == EINT8_23_OFF) {
        switch_handler();
        // handle_led(8, 0);
        // handle_led(11, 1);
        // handle_led(13, 2);
        // handle_led(14, 3);
    } else if (off == INT_TIMER0_OFF) {
        handle_timer0_interrupt();
    }
    SRCPND = 1<<off;
    INTPND = 1<<off;
}

void set_switch_callback(int switch_index, switch_func_t func, void* cb)
{
    if (switch_index < 1 || switch_index > 6) {
        return ;
    }
    switch_conf_t* conf = &_switch_confs[switch_index - 1];
    conf->callback = func;
    conf->cb = cb;
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

uint32_t saved_intmsk;

//禁用中断
void disable_irq(void)
{
    saved_intmsk = INTMSK;
    INTMSK = 0xffffffff;
}

//启用中断
void restore_irq(void)
{
    INTMSK = saved_intmsk;
}