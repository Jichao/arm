#ifndef _LCD_HEADER_
#define _LCD_HEADER_

#include "common.h"

void lcd_init(void);
void lcd_power(BOOL power_inv, BOOL ponwer);
void lcd_enable(BOOL on);
void clear_screen(uint16_t color);

#endif