#include "lcd.h"
#include "common.h"
#include "clock.h"

#define GPCCON (*(volatile uint32_t*)0x56000020)
#define GPCUP (*(volatile uint32_t*)0x56000028)
#define GPDCON (*(volatile uint32_t*)0x56000030)
#define GPDUP (*(volatile uint32_t*)0x56000038)
#define GPGCON (*(volatile uint32_t*)0x56000060)
#define GPGUP (*(volatile uint32_t*)0x56000068)

#define LCDCON1 (*(volatile uint32_t*)0X4D000000)
#define LCDCON2 (*(volatile uint32_t*)0X4D000004)
#define LCDCON3 (*(volatile uint32_t*)0X4D000008)
#define LCDCON4 (*(volatile uint32_t*)0X4D00000c)
#define LCDCON5 (*(volatile uint32_t*)0X4D000010)
#define LCDSADDR1 (*(volatile uint32_t*)0X4D000010)
#define LCDSADDR2 (*(volatile uint32_t*)0X4D000014)
#define LCDSADDR3 (*(volatile uint32_t*)0X4D000018)
#define TPAL (*(volatile uint32_t*)0X4D000050)
#define PALETTE     0x4d000400

#define TD35_VCLK 7.1
#define TD35_VBPD 0
#define TD35_VFPD 0
#define TD35_VSPW 9
#define TD32_HEIGHT 320
#define TD35_LINEVAL (TD32_HEIGHT - 1)

#define TD35_HBPD 100 
#define TD35_HFPD 0
#define TD35_HSPW 4
#define TD32_WIDTH 240
#define TD35_HORZVAL (TD32_WIDTH - 1)

#define TFT_LCD_TYPE 3
#define TFT_8BPP 0xc 

#define ENVID_DISABLE   0
#define ENVID_ENABLE    1

#define FORMAT8BPP_5551 0
#define FORMAT8BPP_565  1

#define VCLK_NORM 0
#define VCLK_INV 1

#define VLINE_NORM 0
#define VLINE_INV 1

#define HSYNC_NORM      0
#define HSYNC_INV       1

#define VSYNC_NORM      0
#define VSYNC_INV       1

#define BSWP            1
#define HWSWP           1

#define VDEN_NORM 0
#define VDEN_INV 0

#define LCDFRAMEBUFFER 0x30800000
#define LOWER21BITS(n)  ((n) & 0x1fffff)

static uint32_t _xsize;
static uint32_t _ysize;
static uint32_t* _frame_buffer;

static void lcd_port_init(void)
{
    GPCUP = 0xffffffff;
    GPDUP = 0xffffffff;
    GPCCON = 0xaaaa02a8;
    GPDCON = 0xaaaaaaaa;
}

static void lcd_palette_init(void)
{
    volatile uint32_t* palette = (uint32_t*)PALETTE;
    for (int i = 0; i < 256; ++i) {
        int r = i % 3;
        uint32_t color;
        switch (r) {
        case 0:
            color = 0b11111;
            break;
        case 1:
            color = 0b111111 << 5;
            break;
        case 2:
            color = 0b11111 << 11;
            break;
        }
        *(palette + i) = color;
    }
}

static void set_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    *(_frame_buffer + y * _xsize + x) = color;
}

void lcd_init(void)
{
    lcd_port_init();

    int clk_div = (int)(get_hclk() / 2. / 7.1) - 1;
    printf("clv div = %d\n", clk_div);

    LCDCON1 = (clk_div << 8) | (TFT_LCD_TYPE << 5) | (TFT_8BPP << 1) | (ENVID_DISABLE);
    LCDCON2 = (TD35_VBPD << 24) | (TD35_LINEVAL << 14) | (TD35_VFPD << 6) | TD35_VSPW;
    LCDCON3 = (TD35_HBPD << 19) | (TD35_HORZVAL << 8) | TD35_HFPD;
    LCDCON4 = TD35_HSPW;
    LCDCON5 = (FORMAT8BPP_565 << 11) | (VCLK_INV << 10) | (VLINE_INV << 9) |
              (VSYNC_INV << 8) | HWSWP;

    LCDSADDR1 = ((LCDFRAMEBUFFER>>22)<<21) | LOWER21BITS(LCDFRAMEBUFFER>>1);
    LCDSADDR2 = LOWER21BITS((LCDFRAMEBUFFER + TD32_WIDTH*TD32_HEIGHT*2)>>1);
    LCDSADDR3 = TD32_WIDTH;
    TPAL = 0;

    lcd_palette_init();
    _xsize = 240;
    _ysize = 320;
    _frame_buffer = (uint32_t*)LCDFRAMEBUFFER;
}

void lcd_power(BOOL invpwren, BOOL pwren)
{
    GPGCON = (GPGCON & (~(3 << 8))) | (3 << 8); // GPG4用作LCD_PWREN
    GPGUP = (GPGUP & (~(1 << 4))) | (1 << 4);   // 禁止内部上拉

    LCDCON5 = (LCDCON5 & (~(1 << 5))) |
              (invpwren << 5); // 设置LCD_PWREN的极性: 正常/反转
    LCDCON5 = (LCDCON5 & (~(1 << 3))) | (pwren << 3); // 设置是否输出LCD_PWREN
}


void lcd_enable(BOOL on)
{
    if (on) {
        LCDCON1 |= 1;
    } else {
        LCDCON1 &= ~1;
    }
}

void clear_screen(uint32_t color) 
{
    for (uint32_t y = 0; y < _ysize; ++y) {
        for (uint32_t x = 0; x < _xsize; ++x) {
            set_pixel(x, y, 1);
        }
    }
}


