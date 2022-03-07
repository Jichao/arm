#include "common.h"
#include "audio.h"
#include "clock.h"

#define	WAVE_FORMAT_PCM 0x0001
#define L3M (1<<2)              //GPB2 = L3MODE
#define L3D (1<<3)              //GPB3 = L3DATA
#define L3C (1<<4)              //GPB4 = L3CLOCK
#define STATUS_ADDR (0x14 + 2)

uint32_t supported_freq[] = {
    8000, 11025, 16000, 22050, 32000, 44100, 48000, 64000,
    88200, 96000
};

static int round(float a) {
    int c = a + 0.5;
    if (c > a) {
        return c;
    } else {
        return c - 1;
    }
}

static void write_l3(uint8_t data, BOOL address)
{
    int i, j;
    if (address) {
        GPBDATA = (GPBDATA & ~(L3D | L3M | L3C)) | L3C; // address mode
    }
    else {
        GPBDATA = (GPBDATA & ~(L3D | L3M | L3C)) | (L3C | L3M); // data mode
    }

    // tsu(l3) > 190ns
    for (j = 0; j < 4; j++) ; 

    for (i = 0; i < 8; i++) {
        if (data & 0x1) // if data's LSB is 'H'
        {
            GPBDATA &= ~L3C; // L3C=L
            GPBDATA |= L3D;  // L3D=H
            // tcy(L3) > 500ns
            for (j = 0; j < 4; j++) ;                   
            GPBDATA |= (L3C | L3D); // L3C=H,L3D=H
            // tcy(L3) > 500ns
            for (j = 0; j < 4; j++) ; 
        }
        else // If data's LSB is 'L'
        {
            GPBDATA &= ~L3C; // L3C=L
            GPBDATA &= ~L3D; // L3D=L
            // tcy(L3) > 500ns
            for (j = 0; j < 4; j++) ;            
            GPBDATA |= L3C;  // L3C=H
            GPBDATA &= ~L3D; // L3D=L
            // tcy(L3) > 500ns
            for (j = 0; j < 4; j++) ; 
        }
        data >>= 1; // For check next bit
    }
    GPBDATA = (GPBDATA & ~(L3D | L3M | L3C)) | (L3C | L3M); // L3M=H,L3C=H
}

static void write_l3_addr(uint8_t addr)
{
    write_l3(addr, TRUE);
}

static void write_l3_data(uint8_t data)
{
    write_l3(data, FALSE);
}

void init_1314()
{
    //l3-bus configuration
    //gpb2-4: l3mode | l3data | l3clock
    GPBCONF = (GPBCONF & (0x3f << 4)) | (0x15 << 4); //gpb2-4 output
    GPBUP = (GPBUP & ~(0x7 << 2)) | (0x7 << 2);
    GPBDATA = (GPBDATA & ~(L3M|L3C|L3D)) |(L3M|L3C);  ///Start condition : L3M=H, L3C=H

    write_l3_addr(STATUS_ADDR);
    write_l3_data(0x50);    //reset,384fs,no dc-flitering,iis-bus
    write_l3_addr(STATUS_ADDR);
    write_l3_data(0x10);    //384fs, no dc-flit3ering,iis-bus
    write_l3_addr(STATUS_ADDR);
    write_l3_data(0xc1);    //dac gain 6db; dac on adc off
}

void init_iis(uint32_t fs, uint32_t bits_per_sample, uint32_t channels)
{
    //iis psr
    uint32_t codeclk = fs * 384;
    int divider = round(get_pclk() * 1000 * 1000. / codeclk) - 1; 
    printf("divider = %d\r\n", divider);
    IISPSR = divider << 5 | divider;

    //gpio configuration
    GPEUP = (GPEUP & ~0x1f) | 0x1f;
    GPECON = (GPECON & ~0x3ff) | 0x2aa;

    //iis configuration
    //tx dma no: rx dma no: tx idle no: rx idel yes:iis prescaler yes: iis start no
    IISCON = (0 << 5) | (0 << 4) | (0 << 3) | (1 << 2) | (1 << 1) | 0;
    //pclk : master mode: tx mode: low left: iis format: serial_bit : master clock freq 384fs: 
    int serial_bit = (bits_per_sample == 16 ? 1 : 0);
    int serial_bit_freq = bits_per_sample * channels;
    switch (serial_bit_freq) {
    case 16:
        serial_bit_freq = 0;
        break;
    case 32:
        serial_bit_freq = 1;
        break;
    case 48:
        serial_bit_freq = 2;
    default:
        printf("invalid serial_bit freq\r\n");
        break;
    }
    printf("serial bit = %d, serial bit freq = %d\r\n", serial_bit, serial_bit_freq);
    IISMOD = (0 << 9) | (0 << 8) | (2 << 6) | (0 << 5) | (0 << 4) | (serial_bit << 3) | (1 << 2) | serial_bit_freq;
    //tx fifo enable
    IISFCON = 1 << 13;
}

int verify_wav_format(wav_format_t* wav)
{
    if (wav->format != WAVE_FORMAT_PCM) {
        return -1;;
    }

    //do the chip support 8 bit wav?
    if (wav->bits_per_sample % 8 != 0)  {
        return -1;
    }

    int i = 0;
    for (; i < jcountof(supported_freq); ++i) {
        if (wav->sample_rate == supported_freq[i]) {
            break;
        }
    }
    if (i == jcountof(supported_freq)) {
        return -1;
    }

    if (wav->bits_per_sample * wav->channels / 8 != wav->block_align) {
        return -1;
    }
    if (wav->bits_per_sample * wav->channels * wav->sample_rate /8 != wav->avg_bytes_per_sec) {
        return-1;
    }
    return 0;
}

int start_play_audio(wav_format_t* wav)
{
    printf("start play audio\r\n");
    int result = verify_wav_format(wav);
    if (result != 0) {
        printf("wav format invalid\r\n");
        return result; 
    }

    init_1314();
    init_iis(wav->sample_rate, wav->bits_per_sample, wav->channels);

    IISCON |= 1;
    for (int i = 0; i < wav->data_len; i += 64) {
        while (IISCON & (1 << 7));
        for (int j = 0; j < 32; ++j) {
            IISFIFO = wav->data[i + 2 * j] + (wav->data[i + 2 * j + 1] << 8);
        }
    }
    IISCON &= ~1;
    printf("play audio done\r\n");
    return 0;
}

