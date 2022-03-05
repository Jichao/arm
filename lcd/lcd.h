#ifndef _LCD_HEADER_
#define _LCD_HEADER_

#include "common.h"

void lcd_init(void);
void lcd_uninit(void);
void lcd_power(BOOL power_inv, BOOL ponwer);
void lcd_enable(BOOL on);
void clear_screen(uint16_t color);
void test_rect(void);
void draw_24bmp(uint8_t* bytes, int width, int height, int offx, int offy);
void draw_16bmp(uint8_t* bytes, int width, int height, int offx, int offy);
#endif