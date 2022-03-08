#include "lcd.h"
#include "common.h"
#include "clock.h"
#include "kmalloc.h"

#define TD35_VCLK 7.1
#define TD35_VBPD 0
#define TD35_VFPD 0
#define TD35_VSPW 9
#define TD35_HEIGHT 320
#define TD35_LINEVAL (TD35_HEIGHT - 1)

#define TD35_HBPD 100 
#define TD35_HFPD 0
#define TD35_HSPW 4
#define TD35_WIDTH 240
#define TD35_HORZVAL (TD35_WIDTH - 1)

#define TFT_LCD_TYPE 3
#define TFT_16BPP 0xc 

#define ENVID_DISABLE   0
#define ENVID_ENABLE    1

#define FORMAT_16BPP_5551 0
#define FORMAT_16BPP_565  1

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

#define LOWER21BITS(n)  ((n) & 0x1fffff)

uint32_t _framebuffer;

static void lcd_port_init(void)
{
    GPCUP = 0xffffffff;
    GPDUP = 0xffffffff;
    GPCCON = 0xaaaa02a8;
    GPDCON = 0xaaaaaaaa;
}

void lcd_uninit()
{
	kfree((void*)_framebuffer);
}

void lcd_init(void)
{
    lcd_port_init();

    int clk_div = (int)(get_hclk() / 2. / TD35_VCLK) - 1;
	printf("hclk = %d\r\n", get_hclk());
    printf("clv div = %d\r\n", clk_div);

	_framebuffer = (uint32_t)kmalloc(2 * TD35_WIDTH * TD35_HEIGHT);
	printf("frame buffer = 0x%x\r\n", _framebuffer);
    LCDCON1 = (clk_div << 8) | (TFT_LCD_TYPE << 5) | (TFT_16BPP << 1) | (ENVID_DISABLE);
    LCDCON2 = (TD35_VBPD << 24) | (TD35_LINEVAL << 14) | (TD35_VFPD << 6) | TD35_VSPW;
    LCDCON3 = (TD35_HBPD << 19) | (TD35_HORZVAL << 8) | TD35_HFPD;
    LCDCON4 = TD35_HSPW;
    LCDCON5 = (FORMAT_16BPP_565 << 11) | (VCLK_INV << 10) | (VLINE_INV << 9) |
              (VSYNC_INV << 8) | HWSWP;

    LCDSADDR1 = ((_framebuffer>>22)<<21) | LOWER21BITS(_framebuffer>>1);
    LCDSADDR2 = LOWER21BITS((_framebuffer + TD35_WIDTH*TD35_HEIGHT*2)>>1);
    LCDSADDR3 = TD35_WIDTH;

    LCDINTMSK |= 3;
  	TCONSEL   &= (~7);
    TPAL     = 0x0;
   	TCONSEL &= ~((1<<4) | 1);
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

static void set_pixel(uint32_t x, uint32_t y, uint16_t c)
{
    if (x < TD35_WIDTH && y < TD35_HEIGHT) {
        uint16_t *addr = (uint16_t *)_framebuffer + y * TD35_WIDTH + x;
        *addr = c;
    } else {
		printf("set_pixel invalid range x = %d, y = %d\r\n", x, y);
	}
}

void draw_16bmp(uint8_t* bytes, int width, int height, int offx, int offy)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint8_t* pixel = (uint8_t*)(bytes + (y * width + x) * 2);
			set_pixel(x + offx, y + offy, (pixel[0] << 8) | pixel[1]);
		}
	}
}

void draw_24bmp(uint8_t* bytes, int width, int height, int offx, int offy)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
			uint8_t* pixel = (uint8_t*)(bytes + (y * width + x) * 3);
			int r = pixel[0] / 255. * ((1 << 5) - 1);
			int g = pixel[1] / 255. * ((1 << 6) - 1);
			int b = pixel[2] / 255. * ((1 << 5) - 1);
			uint16_t c = (r << 11 | g << 5 | b);
			set_pixel(x + offx, y + offy, c);
        }
    }
}

void clear_screen(uint16_t color) {
	for (int y = 0; y < TD35_HEIGHT; ++y) {
		for (int x = 0; x < TD35_WIDTH; ++x) {
			set_pixel(x, y, color);
		}
	}
}

static void Glib_Line(int x1,int y1,int x2,int y2, uint16_t color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					set_pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					set_pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					set_pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					set_pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					set_pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					set_pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					set_pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					set_pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}

static void Glib_FilledRectangle(int x1,int y1,int x2,int y2, uint16_t color)
{
    int i;
    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}

void test_rect(void)
{
#define LCD_BLANK 30
#define C_UP (TD35_WIDTH - LCD_BLANK * 2)
#define C_RIGHT (TD35_WIDTH - LCD_BLANK * 2)
#define V_BLACK ((TD35_HEIGHT - LCD_BLANK * 4) / 6)

    Glib_FilledRectangle(LCD_BLANK, LCD_BLANK, (TD35_WIDTH - LCD_BLANK),
                         (TD35_HEIGHT - LCD_BLANK),
                         0x0000); // fill a Rectangle with some color

    Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 0),
                         (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 1),
                         0x001f); // fill a Rectangle with some color
    Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 1),
                         (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 2),
                         0x07e0); // fill a Rectangle with some color
    Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 2),
                         (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 3),
                         0xf800); // fill a Rectangle with some color
    Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 3),
                         (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 4),
                         0xffe0); // fill a Rectangle with some color
    Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 4),
                         (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 5),
                         0xf81f); // fill a Rectangle with some color
    Glib_FilledRectangle((LCD_BLANK * 2), (LCD_BLANK * 2 + V_BLACK * 5),
                         (C_RIGHT), (LCD_BLANK * 2 + V_BLACK * 6),
                         0x07ff); // fill a Rectangle with some color
}
