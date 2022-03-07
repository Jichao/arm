#include "common.h"
#include "audio.h"
#include "clock.h"

#define	WAVE_FORMAT_PCM 0x0001
#define L3M (1<<2)              //GPB2 = L3MODE
#define L3D (1<<3)              //GPB3 = L3DATA
#define L3C (1<<4)              //GPB4 = L3CLOCK

uint32_t supported_freq[] = {
    8000, 11025, 16000, 22050, 32000, 44100, 48000, 64000,
    88200, 96000
};

int start_play_audio(wav_format_t* wav)
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
}


static void write_l3(uint8_t data, BOOL address)
{
    
}

void init_1314()
{
    //l3-bus configuration
    //gpb2-4: l3mode | l3data | l3clock
    GPBCONF = GPBCONF & (0x3f << 4) | (0x15 << 4); //gpb2-4 output
    GPBUP = GPBUP & ~(0x7 << 2) | (0x7 << 2);


}

void init_iis(uint32_t fs, uint32_t bits_per_sample, uint32_t channels)
{
    //iis psr
    uint32_t codeclk = fs * 384;
    int divider = get_pclk() * 1000 * 1000 / codeclk - 1; 
    IISPSR = divider << 5 | divider;

    //gpio configuration
    GPEUP = GPEUP & ~0x1f | 0x1f;
    GPECON = GPECON & ~0x3ff | 0x2aa;

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
        serial_bit = 1;
        break;
    case 48:
        serial_bit = 2;
    default:
        printf("invalid serial_bit freq\r\n");
        break;
    }
    IISMOD = (0 << 9) | (0 << 8) | (2 << 6) | (0 << 5) | (0 << 4) | (serial_bit << 3) | (1 << 2) | serial_bit_freq;
    IISFCON = 1 << 13;
}

