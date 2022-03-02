#include "common.h"
#include "nand.h"

#define NFCONF (*(volatile uint32_t*)0x4E000000)
#define NFCONT (*(volatile uint32_t*)0x4E000004)
#define NFCMMD (*(volatile uint8_t*)0x4E000008)
#define NFADDR (*(volatile uint8_t*)0x4E00000C)
#define NFDATA (*(volatile uint8_t*)0x4E000010)

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

#define NAND_PAGE_SIZE 2048
#define NAND_PAGE_ADDR_MASK (NAND_PAGE_SIZE - 1)

//mini2440
//fclk:最高支持400MHz
//hclk(ahb bus):给soc上的外设用的 400/3.
//pclk(apb bus):给其他外设用的

//133.3MHz
//minimum 25ns
#define TACLS 0  //0ns
#define TWRPH0 2 //3clk: 3*7.5=15
#define TWRPH1 0 //1clk: 7.5

static void manual_wait_cycle(volatile int i)
{
    for (i = 0; i < 10; ++i);
}

static void nand_select_chip(void) 
{
    NFCONT &= ~(1<<1);
}

static void nand_unselect_chip(void)
{
    NFCONT |= (1<<1);
}

static void nand_clear_rnb(void)
{
    NFSTAT |= (1<<2);
}

static void nand_wait_rnb(void)
{
    while (!(NFSTAT & (1 << 2)));
}

static void nand_wait_ready(void)
{
    while ((NFSTAT & 0x1) == 0) ;
}

static void nand_write_cmd(uint8_t cmd) 
{
    NFCMMD = cmd;
}

static void nand_write_addr(uint8_t addr) 
{
    NFADDR = addr;
}

static void nand_write_addr32(uint32_t addr)
{
    uint32_t col = addr & NAND_PAGE_ADDR_MASK;
    uint32_t page = addr / NAND_PAGE_SIZE;
    NFADDR = col & 0xff; //a0-a7
    NFADDR = (col >> 8) & 0xf; //a8-a11
    NFADDR = page & 0xff; //a12-a19
    NFADDR = (page >> 8) & 0xff; //a20-a27
    NFADDR = (page >> 16) & 0x3; //a28
}

static void nand_reset(void)
{
    nand_select_chip();
    manual_wait_cycle(10);
    nand_write_cmd(0xff);
    nand_wait_ready();
    nand_unselect_chip();
}

uint32_t nand_read_dev_id(void)
{
    uint32_t dev_id;
    nand_select_chip();
    nand_write_cmd(0x90);
    nand_write_addr(0x00);
    nand_wait_ready();
    dev_id = NFDATA;
    dev_id = NFDATA;
    dev_id |= NFDATA << 8;
    dev_id |= NFDATA << 16;
    dev_id |= NFDATA << 24;
    nand_unselect_chip();
    return dev_id;
}

void nand_init(void)
{
    NFCONF = (TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4);
    NFCONT = (0<<13)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(0<<6)|(0<<5)|(1<<4)|(1<<1)|(1<<0);    
    NFSTAT = 0;
    nand_reset();
}

int nand_to_ram(uint32_t start_addr, uint32_t size, uint8_t* ram_addr)
{
    if (!ram_addr) {
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
        nand_reset();
        nand_select_chip();
        nand_write_cmd(0x00);
        nand_write_addr32(addr);
        nand_write_cmd(0x30);
        nand_wait_ready();
        for (uint32_t i = 0; i < page_size; i++) {
            *ram_addr = NFDATA;
            ram_addr++;
        }
        nand_unselect_chip();
    }
    return 0;
}
