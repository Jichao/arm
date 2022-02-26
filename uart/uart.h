#ifndef _UART_HEADER_
#define _UART_HEADER_

void uart_init(int baudrate);
void uart_send_string(const char* string);
int uart_get_string(char* buff, int buff_size);

void on_uart_rx_ready();
void on_uart_tx_ready();

#endif