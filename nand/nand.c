#include "common.h"
#include "nand.h"

#define NFCONF (*(volatile uint32_t*)0x4E000000)
#define NFCONT (*(volatile uint32_t*)0x4E000004)
#define NFCMMD (*(volatile uint8_t*)0x4E000008)
#define NFADDR (*(volatile uint8_t*)0x4E00000C)
#define NFDATA (*(volatile uint32_t*)0x4E000010)

#define NFMECCD0 (*(volatile uint32_t*)0x4E000014)
#define NFMECCD1 (*(volatile uint32_t*)0x4E000018)
#define NFSECCD (*(volatile uint32_t*)0x4E00001c)
#define NFSTAT (*(volatile uint8_t*)0x4E000020)
#define NFESTAT0 (*(volatile uint32_t*)0x4E000024)
#define NFESTAT1 (*(volatile uint32_t*)0x4E000028)
#define NFMECC0 (*(volatile uint32_t*)0x4E00002c)
#define NFMECC1 (*(volatile uint32_t*)0x4E000030)
#define NFSBLK (*(volatile uint32_t*)0x4E000034)
#define NFEBLK (*(volatile uint32_t*)0x4E000038)

static void __attribute__((optimize("O0"))) manual_wait_cycle(void)
{
    for (int i = 0; i < 10; ++i);
}

static void nand_select_chip(void) 
{
    NFCONT &= ~(1<<1);
    manual_wait_cycle();
}

static void nand_unselect_chip(void)
{
    NFCONT |= (1<<1);
}

static void nand_wait_ready(void)
{
    while ((NFSTAT & 0x1) == 0) ;
}

static void nand_write_cmd(uint8_t cmd) 
{
    NFCMMD = cmd;
}

static void nand_reset(void)
{
    nand_select_chip();
    nand_write_cmd(0xff);
    nand_wait_ready();
    nand_unselect_chip();
}

static void nand_write_address(uint32_t addr)
{
    NFADDR = addr & 0xff;
    manual_wait_cycle();
    NFADDR = ((addr >> 8) & 0xf);
    manual_wait_cycle();
    NFADDR = ((addr >> 12) & 0xff);
    manual_wait_cycle();
    NFADDR = ((addr >> 20) & 0xff);
    manual_wait_cycle();
    NFADDR = ((addr >> 28) & 0x1);
    manual_wait_cycle();
}

static void nand_page_read(uint32_t page_addr, uint32_t page_size, uint8_t* output)
{
    nand_select_chip();
    nand_write_cmd(0x00);
    nand_write_address(page_addr);
    nand_write_cmd(0x30);
    nand_wait_ready();
    for (uint32_t i = 0; i < page_size; i += 4) {
        *((uint32_t*)(output + i)) = NFDATA;
    }
    nand_unselect_chip();
}

void nand_init(void)
{
    //set tacls: 1
    NFCONF |= 1 << 12;
    //enable nand controller & unselect chip
    NFCONT |= B8(00010011);
    nand_reset();
}

int nand_read(uint32_t start_addr, uint32_t size, uint8_t* ram_buffer)
{
    if (!ram_buffer) {
        return -1;
    }
    if (size == 0) {
        return 0;
    }
    uint32_t page_size = 2048;
    if (start_addr % page_size != 0) {
        return -1;
    }   
    if (size % page_size != 0) {
        return -1;
    }
    for (uint32_t addr = start_addr; addr < start_addr + size; addr += page_size) {
        nand_page_read(addr, page_size, ram_buffer);
        ram_buffer += page_size;
    }
    return size;
}
