#include "ram.h"
#include "nand.h"
#include "led.h"

typedef void (*t_void_func)(void);

int main(void)
{
    ram_init();
    nand_init();
    volatile uint32_t dev_id = nand_read_dev_id();
    dev_id += 3;
    int result = nand_to_ram(4096, 2048, (uint8_t*)SDRAM_CODE_BASE);
    if (result == 0) {
        __asm__("ldr sp, =0x34000000" : :);
        volatile t_void_func ptr = &led_control;
        (*ptr)();
    } 
    return 0;
}
