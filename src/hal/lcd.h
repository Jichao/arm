#ifndef _LCD_HEADER_
#define _LCD_HEADER_

#include "base/types.h"

void lcd_init(void);
void lcd_uninit(void);
void lcd_power(bool power_inv, bool ponwer);
void lcd_enable(bool on);
void clear_screen(uint16_t color);
void test_rect(void);
void draw_24bmp(uint8_t* bytes, int width, int height, int offx, int offy);
void draw_16bmp(uint8_t* bytes, int width, int height, int offx, int offy);

#endif