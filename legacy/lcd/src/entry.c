#include "entry.h"
#include "clock.h"
#include "kmalloc.h"
#include "led.h"
#include "int.h"
#include "timer.h"
#include "common.h"
#include "uart.h"
#include "stdio.h"
#include "lcd.h"
#include "girl.h"
#include "audio.h"
#include "wav.h"
#include "apple_wav.h"

extern char _ram_start;
extern char _bss_end;

void on_timer(void)
{
    invert_led(0);
}

void test_lcd(void)
{
    printf("start test lcd\r\n");

    lcd_init();
    lcd_power(0, 1);
    lcd_enable(TRUE);

    printf("clear lcd with red\r\n");
    clear_screen((0b11111 << 11));
    getc();

    printf("draw rect\r\n");
    test_rect();
    getc();

    printf("draw girl\r\n");
    draw_24bmp(girl_180x320, 180, 320, 30, 0);
    getc();

    printf("lcd test done turning off\r\n");
    lcd_enable(FALSE);
    lcd_power(0, 0);
    lcd_uninit();
}

void test_audio(void)
{
    wav_format_t* wav = read_wav_file(apple_wav_file, 1764098);
    if (!wav) {
        printf("read wav file failed\r\n");
        return ;
    }
    start_play_audio(wav);
    kfree(wav);
}

void entry(void)
{
    printf("bbs end %p ram start %p rom size %x\r\n",
        &_bss_end, &_ram_start, ((int)&_bss_end - (int)&_ram_start));
    kmalloc_init();

    printf("led inited\r\n");
    led_init();

    printf("fclk %d hclk %d pclk %d\r\n", get_fclk(), get_hclk(), get_pclk());
    printf("before interrupt inited\r\n");
    interrupt_init();
    printf("interupt inited\r\n");

    set_timer0(2*1000, TRUE, &on_timer);
    printf("timer0 inited\r\n");

    while (1) {
        printf("function menu \r\n");
        printf("  1. lcd test\r\n");
        printf("  2. music test\r\n");
        char choice = getc();
        if (choice == '1') {
            test_lcd();
        }
        else if (choice == '2') {
            test_audio();
        }
    }
}