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
#define LCDINTMSK  (*(volatile unsigned *)0x4d00005c)	//LCD Interrupt mask
#define TCONSEL     (*(volatile unsigned *)0x4d000060)	//LPC3600 Control --- edited by junon
#define PALETTE     0x4d000400

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


//new
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

#define LCD_PIXCLOCK 4

#define LCD_RIGHT_MARGIN 100
#define LCD_LEFT_MARGIN 0
#define LCD_HSYNC_LEN 4

#define LCD_UPPER_MARGIN 0
#define LCD_LOWER_MARGIN 0
#define LCD_VSYNC_LEN 9

#define LCD_XSIZE  LCD_WIDTH
#define LCD_YSIZE  LCD_HEIGHT
#define SCR_XSIZE  LCD_WIDTH
#define SCR_YSIZE  LCD_HEIGHT
//new

typedef uint32_t U32;
typedef uint16_t U16;

volatile static unsigned short LCD_BUFFER[TD35_HEIGHT][TD35_WIDTH];

#define LOWER21BITS(n)  ((n) & 0x1fffff)

static uint32_t _xsize;
static uint32_t _ysize;
static uint16_t* _frame_buffer;

static void lcd_port_init(void)
{
    GPCUP = 0xffffffff;
    GPDUP = 0xffffffff;
    GPCCON = 0xaaaa02a8;
    GPDCON = 0xaaaaaaaa;
}

void lcd_init(void)
{
    lcd_port_init();

    int clk_div = (int)(get_hclk() / 2. / 10) - 1;
    printf("clv div = %d\n", clk_div);

    LCDCON1 = (6 << 8) | (TFT_LCD_TYPE << 5) | (TFT_16BPP << 1) | (ENVID_DISABLE);
    LCDCON2 = (TD35_VBPD << 24) | (TD35_LINEVAL << 14) | (TD35_VFPD << 6) | TD35_VSPW;
    LCDCON3 = (TD35_HBPD << 19) | (TD35_HORZVAL << 8) | TD35_HFPD;
    LCDCON4 = TD35_HSPW;
    LCDCON5 = (FORMAT_16BPP_565 << 11) | (VCLK_INV << 10) | (VLINE_INV << 9) |
              (VSYNC_INV << 8) | HWSWP;

#define LCD_ADDR ((uint32_t)LCD_BUFFER)

    LCDSADDR1 = ((LCD_ADDR>>22)<<21) | LOWER21BITS(LCD_ADDR>>1);
    LCDSADDR2 = LOWER21BITS((LCD_ADDR + TD35_WIDTH*TD35_HEIGHT*2)>>1);
    LCDSADDR3 = TD35_WIDTH;

    LCDINTMSK |= 3;
  	TCONSEL   &= (~7);
    TPAL     = 0x0;
   	TCONSEL &= ~((1<<4) | 1);

    _xsize = 240;
    _ysize = 320;
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

static void PutPixel(U32 x,U32 y,U16 c)
{
    if(x<SCR_XSIZE && y<SCR_YSIZE)
		LCD_BUFFER[(y)][(x)] = c;
}


static void Glib_Line(int x1,int y1,int x2,int y2, U16 color)
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
					PutPixel(x1,y1,color);
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
					PutPixel(x1,y1,color);
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
					PutPixel(x1,y1,color);
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
					PutPixel(x1,y1,color);
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
					PutPixel(x1,y1,color);
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
					PutPixel(x1,y1,color);
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
					PutPixel(x1,y1,color);
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
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}


/**************************************************************
在LCD屏幕上用颜色填充一个矩形
**************************************************************/
static void Glib_FilledRectangle(int x1,int y1,int x2,int y2, U16 color)
{
    int i;

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}

void clear_screen(uint16_t color) 
{
    	#define LCD_BLANK		30
	#define C_UP		( LCD_XSIZE - LCD_BLANK*2 )
	#define C_RIGHT		( LCD_XSIZE - LCD_BLANK*2 )
	#define V_BLACK		( ( LCD_YSIZE - LCD_BLANK*4 ) / 6 )
	Glib_FilledRectangle( LCD_BLANK, LCD_BLANK, ( LCD_XSIZE - LCD_BLANK ), ( LCD_YSIZE - LCD_BLANK ),0x0000);		//fill a Rectangle with some color

	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*0), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*1),0x001f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*1), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*2),0x07e0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*2), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*3),0xf800);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*3), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*4),0xffe0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*4), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*5),0xf81f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*5), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*6),0x07ff);		//fill a Rectangle with some color
    // for (uint32_t y = 0; y < _ysize; ++y) {
    //     for (uint32_t x = 0; x < _xsize; ++x) {
    //         LCD_BUFFER[y][x] = color;
    //     }
    // }
}

