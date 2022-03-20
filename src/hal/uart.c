#include "base/common.h"
#include "uart.h"
#include "clock.h"

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
    UBRDIV0 = (int)(get_pclk() * 1000 * 1000 / (baudrate * 16)) - 1;
}

unsigned char getc(void)
{
    while ((UTRSTAT0 & 0x1) == 0);
    return URXH0;
}

void putc(unsigned char ch)
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
        putc(ch);
        *buff = ch;
        buff++;
        size++;
    } while (ch != '\r' && ch != '\n' && size < out_size - 1);
    return size;
}

