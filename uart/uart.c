#include "common.h"
#include "uart.h"
#include "clock.h"

#define GPHCON (*(volatile uint32_t*)0x56000070)
#define GPHUP (*(volatile uint32_t*)0x56000078)
#define ULCON0 (*(volatile uint32_t*)0x50000000)
#define UCON0 (*(volatile uint32_t*)0x50000004)
#define UFCON0 (*(volatile uint32_t*)0x50000008)
#define UMCON0 (*(volatile uint32_t*)0x5000000c)
#define UTRSTAT0 (*(volatile uint32_t*)0x50000010)
#define UMSTAT0 (*(volatile uint32_t*)0x5000001C)
#define UBRDIV0 (*(volatile uint32_t*)0x50000028)
#define UTXH0 (*(volatile uint8_t*)0x50000020)
#define URXH0 (*(volatile uint8_t*)0x50000024)
#define UBRDIV0 (*(volatile uint32_t*)0x50000028)

//init uart0
void uart_init(int baudrate)
{
    //gph2-txd0 gph3-rxd0 disable 2,3 pull up function
    GPHCON &= ~0xf0;
    GPHCON |= 0xa0;
    GPHUP = 0xc; 

    ULCON0 = 0x3; //no parity,8-bit,one stop bit,normal mode.
    UFCON0 = 0x0; //disable fifo mode
    UCON0 = 0x5; //enable tx/rx 
    UMCON0 = 0x0; //disable afc
    UBRDIV0 = 36;
}

static char getc(void)
{
    while ((UTRSTAT0 & 0x1) == 0);
    return URXH0;
}

static void putc(char ch)
{
    while ((UTRSTAT0 & (1<<2)) == 0);
    UTXH0 = ch;
}

void uart_send_string(const char* str) 
{
    if (!str) {
        return;
    }
    while (*str) {
        putc(*str);
        str++;
    }
}

int uart_get_string(char* buff, int out_size)
{
    int size = 0;
    char ch;
    do {
        ch = getc();
        *buff = ch;
        buff++;
        size++;
    } while (ch != '\r' && ch != '\n' && size < out_size - 1);
    return size;
}