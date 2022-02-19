#include "ram.h"
#include "nand.h"
#include "led.h"

typedef void (*t_void_func)(void);

int main(void)
{
    ram_init();
    nand_init();
    nand_read(4096, 2048, (uint32_t*)SDRAM_CODE_BASE);

    __asm__ (
        "ldr sp, =0x34000000"
        :
        :
    );

    volatile t_void_func ptr = &led_control;
    (*ptr)();
    return 0;
}
