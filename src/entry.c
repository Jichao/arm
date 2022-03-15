#include "entry.h"
#include "apple_wav.h"
#include "audio.h"
#include "clock.h"
#include "common.h"
#include "girl.h"
#include "int.h"
#include "kmalloc.h"
#include "lcd.h"
#include "led.h"
#include "mp3.h"
#include "stdio.h"
#include "timer.h"
#include "uart.h"
#include "wav.h"

extern char _ram_start;
extern char _bss_end;

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

void play_wav(void)
{
    wav_format_t *wav = read_wav_file(apple_wav_file, 1764098);
    if (!wav) {
        printf("read wav file failed\r\n");
        return;
    }
    start_play_audio(wav);
    kfree(wav);
}

void play_mp3(BOOL direct)
{
    printf("start play mp3...\r\n");
    start_play_mp3(apple_mp3, apple_mp3_size(), direct);
    printf("end play mp3...\r\n");
}

ktimer_t *timer1;
ktimer_t *timer2;

void on_switch1(void *cb)
{
    printf("switch 1 switched\r\n");
    destroy_timer(timer1);
    timer1 = NULL;
}

void on_timer1(void *cb)
{
    dprintk("%u ===== on timer 1 called =====\r\n", get_tick_count());
    invert_led((int)cb);
}

void on_timer2(void *cb)
{
    dprintk("%u ===== on timer 2 called =====\r\n", get_tick_count());
}

void test_timer(void)
{
    dprintk("start test timer...\r\n");
    timer1 =
        create_timer(2000, TRUE, (timer_callback_t)&on_timer1, (void *)1, TRUE);
    timer2 = create_timer(2000, TRUE, (timer_callback_t)&on_timer2, NULL, TRUE);
    set_switch_callback(1, &on_switch1, NULL);
}

void end_timer(void)
{
    if (timer1) {
        destroy_timer(timer1);
        timer1 = NULL;
    }

    if (timer2) {
        destroy_timer(timer2);
        timer2 = NULL;
    }
}

void entry(void)
{
    printf("bbs end %p ram start %p rom size %x\r\n", &_bss_end, &_ram_start,
           ((int)&_bss_end - (int)&_ram_start));
    kmalloc_init();

    init_timer();

    printf("led inited\r\n");
    led_init();

    printf("fclk %d hclk %d pclk %d\r\n", get_fclk(), get_hclk(), get_pclk());
    printf("before interrupt inited\r\n");
    interrupt_init();
    printf("interupt inited\r\n");

    int c = 0;

    while (1) {
        printf("  ***function menu*** \r\n");
        printf("  current tick count = %u\r\n", get_tick_count());
        printf("  1. lcd test\r\n");
        printf("  2. play wav test\r\n");
        printf("  3. play mp3 sync test\r\n");
        printf("  4. play mp3 async test\r\n");
        printf("  5. timer test\r\n");

        char choice = getc();
        if (c == 5) {
            printf("timer ended\r\n");
            end_timer();
        }

        c = choice - '0';
        switch (c) {
        case 1:
            test_lcd();
            break;
        case 2:
            play_wav();
            break;
        case 3:
            play_mp3(FALSE);
            break;
        case 4:
            play_mp3(TRUE);
            break;
        case 5:
            test_timer();
            break;
        default:
            break;
        };
    }
}