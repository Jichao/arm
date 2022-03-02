#include "ram.h"
#include "nand.h"
#include "clock.h"
#include "entry.h"

typedef void (*t_void_func)(void);

int raise(void)
{
    return 0;
}

void clock_init(void)
{
    __asm__ (
        "mrc p15,0,r0,c1,c0,0\n"
        "orr r0, r0, #0xc0000000\n"
        "mcr p15,0,r0,c1,c0,0\n"
    );
    CLKDIV = 0x7; //fclk:hclk:pclk = 6:2:1
    MPLLCON = (0x7f << 12 | 2 << 4 | 1);
}

int main(void)
{
    clock_init();
    ram_init();
    __asm__("ldr sp, =0x33ffffe0" : :);
    nand_init();
    uint32_t dev_id = nand_read_dev_id();
    if ((dev_id & 0xda) != 0xda) {
        return 0;
    }
    int result = nand_to_ram(4096, 2048 * 200, (uint8_t*)SDRAM_CODE_BASE);
    if (result == 0) {
        bss_zero();
        entry();
    }
    return result;
}