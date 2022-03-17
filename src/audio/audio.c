#include "audio/audio.h"
#include "hal/clock.h"
#include "audio/mp3.h"
#include "mem/kmalloc.h"
#include "common.h"
#include "mad.h"
#include "timer.h"

#define WAVE_FORMAT_PCM 0x0001
#define L3M (1 << 2) // GPB2 = L3MODE
#define L3D (1 << 3) // GPB3 = L3DATA
#define L3C (1 << 4) // GPB4 = L3CLOCK
#define STATUS_ADDR (0x14 + 2)

bool mp3_direct = 0;

typedef void (*play_func_t)(uint8_t* data);
typedef struct tag_pcm_buffer {
    uint8_t* buf;
    uint32_t buf_size;
    uint32_t ptr;
    play_func_t play_callback;
} pcm_buffer;

uint32_t avg_int = 0;
uint32_t last_tick = 0;

void play_pcm(uint8_t *data)
{
    while (IISCON & (1 << 7)) ;
    for (int j = 0; j < 32; ++j) {
        IISFIFO = data[2 * j] + (data[2 * j + 1] << 8);
    }
}

uint32_t _supported_freqs[] = {8000,  11025, 16000, 22050, 32000,
                               44100, 48000, 64000, 88200, 96000};

static void write_l3(uint8_t data, bool address)
{
    int i, j;
    if (address) {
        GPBDATA = (GPBDATA & ~(L3D | L3M | L3C)) | L3C; // address mode
    }
    else {
        GPBDATA = (GPBDATA & ~(L3D | L3M | L3C)) | (L3C | L3M); // data mode
    }

    // tsu(l3) > 190ns
    for (j = 0; j < 4; j++)
        ;

    for (i = 0; i < 8; i++) {
        if (data & 0x1) // if data's LSB is 'H'
        {
            GPBDATA &= ~L3C; // L3C=L
            GPBDATA |= L3D;  // L3D=H
            // tcy(L3) > 500ns
            for (j = 0; j < 4; j++)
                ;
            GPBDATA |= (L3C | L3D); // L3C=H,L3D=H
            // tcy(L3) > 500ns
            for (j = 0; j < 4; j++)
                ;
        }
        else // If data's LSB is 'L'
        {
            GPBDATA &= ~L3C; // L3C=L
            GPBDATA &= ~L3D; // L3D=L
            // tcy(L3) > 500ns
            for (j = 0; j < 4; j++)
                ;
            GPBDATA |= L3C;  // L3C=H
            GPBDATA &= ~L3D; // L3D=L
            // tcy(L3) > 500ns
            for (j = 0; j < 4; j++)
                ;
        }
        data >>= 1; // For check next bit
    }
    GPBDATA = (GPBDATA & ~(L3D | L3M | L3C)) | (L3C | L3M); // L3M=H,L3C=H
}

static void write_l3_addr(uint8_t addr) { write_l3(addr, TRUE); }

static void write_l3_data(uint8_t data) { write_l3(data, FALSE); }

void init_1314(void)
{
    // l3-bus configuration
    // gpb2-4: l3mode | l3data | l3clock
    GPBCONF = (GPBCONF & (0x3f << 4)) | (0x15 << 4); // gpb2-4 output
    GPBUP = (GPBUP & ~(0x7 << 2)) | (0x7 << 2);
    GPBDATA = (GPBDATA & ~(L3M | L3C | L3D)) |
              (L3M | L3C); /// Start condition : L3M=H, L3C=H

    write_l3_addr(STATUS_ADDR);
    write_l3_data(0x50); // reset,384fs,no dc-flitering,iis-bus
    write_l3_addr(STATUS_ADDR);
    write_l3_data(0x10); // 384fs, no dc-flit3ering,iis-bus
    write_l3_addr(STATUS_ADDR);
    write_l3_data(0xc1); // dac gain 6db; dac on adc off
}

void init_iis(uint32_t fs, uint32_t bits_per_sample, uint32_t channels)
{
    // iis psr
    uint32_t codeclk = fs * 384;
    int divider = roundf(get_pclk() * 1000 * 1000. / codeclk) - 1;
    printf("divider = %d\r\n", divider);
    IISPSR = divider << 5 | divider;

    // gpio configuration
    GPEUP = (GPEUP & ~0x1f) | 0x1f;
    GPECON = (GPECON & ~0x3ff) | 0x2aa;

    // iis configuration
    // tx dma no: rx dma no: tx idle no: rx idel yes:iis prescaler yes: iis
    // start no
    IISCON = (0 << 5) | (0 << 4) | (0 << 3) | (1 << 2) | (1 << 1) | 0;
    // pclk : master mode: tx mode: low left: iis format: serial_bit : master
    // clock freq 384fs:
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
    printf("serial bit = %d, serial bit freq = %d\r\n", serial_bit,
           serial_bit_freq);
    IISMOD = (0 << 9) | (0 << 8) | (2 << 6) | (0 << 5) | (0 << 4) |
             (serial_bit << 3) | (1 << 2) | serial_bit_freq;
    // tx fifo enable
    IISFCON = 1 << 13;
}

static bool is_freq_ok(uint32_t freq)
{
    int i = 0;
    for (; i < jcountof(_supported_freqs); ++i) {
        if (freq == _supported_freqs[i]) {
            break;
        }
    }
    if (i == jcountof(_supported_freqs)) {
        return FALSE;
    }
    return TRUE;
}

int verify_wav_format(wav_format_t *wav)
{
    if (wav->format != WAVE_FORMAT_PCM) {
        return -1;
    }

    if (!is_freq_ok(wav->sample_rate)) {
        return -1;
    }

    // do the chip support 8 bit wav?
    if (wav->bits_per_sample % 8 != 0) {
        return -1;
    }

    if (wav->bits_per_sample * wav->channels / 8 != wav->block_align) {
        return -1;
    }
    if (wav->bits_per_sample * wav->channels * wav->sample_rate / 8 !=
        wav->avg_bytes_per_sec) {
        return -1;
    }
    return 0;
}

enum mad_flow header_callback(void *data, struct mad_header const *header)
{
    if (!is_freq_ok(header->samplerate)) {
        printf("mp3 invalid sample rate\r\n");
        return MAD_FLOW_STOP;
    }

    init_1314();
    // todo: remove hard code
    init_iis(header->samplerate, 16, 2);
    IISCON |= 1;
    return MAD_FLOW_CONTINUE;
}

static inline signed int scale(mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE)
        sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
        sample = -MAD_F_ONE;

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

void pcm_add_sample(pcm_buffer* pb, uint8_t v)
{
    if (pb->ptr >= pb->buf_size) {
        return;
    }
    pb->buf[pb->ptr] = v;
    pb->ptr++;

    if (mp3_direct && pb->ptr == 64) {
        pb->play_callback(pb->buf);
        pb->ptr = 0;
    }
}

pcm_buffer* _pcm;

static enum mad_flow output_callback(void *data,
                                     struct mad_header const *header,
                                     struct mad_pcm *pcm)
{
    uint32_t tick = get_tick_count();
    if (last_tick) {
        uint32_t interval = tick - last_tick;
        if (avg_int == 0) {
            avg_int = interval;
        }
        else {
            avg_int = (avg_int + interval) >> 1;
        }
        last_tick = tick;
    }

    //printf("output callback called\r\n");
    unsigned int nchannels, nsamples;
    mad_fixed_t const *left_ch, *right_ch;

    /* pcm->samplerate contains the sampling frequency */

    nchannels = pcm->channels;
    nsamples = pcm->length;
    left_ch = pcm->samples[0];
    right_ch = pcm->samples[1];

    pcm_buffer* pb = _pcm;

    while (nsamples--) {
        signed int sample;

        /* output sample(s) in 16-bit signed little-endian PCM */
        sample = scale(*left_ch++);
        pcm_add_sample(pb, (sample >> 0) & 0xff);
        pcm_add_sample(pb, (sample >> 8) & 0xff);

        if (nchannels == 2) {
            sample = scale(*right_ch++);
            pcm_add_sample(pb, (sample >> 0) & 0xff);
            pcm_add_sample(pb, (sample >> 8) & 0xff);
        }
    }

    return MAD_FLOW_CONTINUE;
}

pcm_buffer* create_pcm_buffer(int size)
{
    pcm_buffer* pcm = (pcm_buffer*)kmalloc(sizeof(pcm_buffer));
    pcm->buf = (uint8_t*)kmalloc(size);
    pcm->buf_size = size;
    pcm->ptr = 0;
    pcm->play_callback = &play_pcm;
    return pcm;
}

void free_pcm_buffer(pcm_buffer* pcm) {
    kfree(pcm->buf);
    kfree(pcm);
}

int play_mp3_async(const unsigned char *buff, int size)
{
    printf("ASYNC start play mp3 audio, buffer = %p, size = %d\r\n", buff, size);
    _pcm = create_pcm_buffer(2 << 20);

    init_1314();
    // todo: remove hard code
    init_iis(44100, 16, 2);
    IISCON |= 1;
    uint32_t tick = get_tick_count();
    decode_mp3(buff, size, 0, output_callback, 0);
    uint32_t interval  = get_tick_count() - tick;

    IISCON &= ~1;
    free_pcm_buffer(_pcm);
    printf("end play mp3 audio output avg_tick = %d whole time = %d\r\n", avg_int, interval);
    return 0;
}

int play_mp3_sync(const unsigned char *buff, int size)
{
    uint32_t xx_tick, xx_interval;
    printf("SYNC start play mp3 audio, buffer = %p, size = %d\r\n", buff, size);
    _pcm = create_pcm_buffer(2 << 20);

    printf("decoding mp3....\r\n");
    xx_tick = get_tick_count();
    decode_mp3(buff, size, 0, output_callback, 0);
    xx_interval  = get_tick_count() - xx_tick;
    printf("decode mp3 done tick count = %d....\r\n", xx_interval);

    init_1314();
    // todo: remove hard code
    init_iis(44100, 16, 2);
    IISCON |= 1;

    xx_tick = get_tick_count();
    for (int i = 0; i < _pcm->ptr; i += 64) {
        uint32_t t = get_tick_count();
        while (IISCON & (1 << 7))
            ;
        for (int j = 0; j < 32; ++j) {
            IISFIFO = _pcm->buf[i + 2 * j] + (_pcm->buf[i + 2 * j + 1] << 8);
        }
        uint32_t interval = get_tick_count() - t;
        if (avg_int == 0) {
            avg_int = interval;
        }
        else {
            avg_int = (avg_int + interval) >> 1;
        }
    }
    xx_interval  = get_tick_count() - xx_tick;
    IISCON &= ~1;
    free_pcm_buffer(_pcm);
    printf("end play mp3 audio play avg feed interval %d whole time = %d\r\n", avg_int, xx_interval);
    return 0;
}


int start_play_mp3(const unsigned char *buff, int size, bool direct)
{
    mp3_direct = direct;
    if (mp3_direct)
        return play_mp3_async(buff, size);
    else 
        return play_mp3_sync(buff, size);
}

int start_play_audio(wav_format_t *wav)
{
    printf("start play wav audio\r\n");
    int result = verify_wav_format(wav);
    if (result != 0) {
        printf("wav format invalid\r\n");
        return result;
    }

    init_1314();
    init_iis(wav->sample_rate, wav->bits_per_sample, wav->channels);

    printf("wav data len = %d\r\n", wav->data_len);

    IISCON |= 1;
    for (int i = 0; i < wav->data_len; i += 64) {
        while (IISCON & (1 << 7))
            ;
        for (int j = 0; j < 32; ++j) {
            IISFIFO = wav->data[i + 2 * j] + (wav->data[i + 2 * j + 1] << 8);
        }
        /*
        for (int j = 0; j < 20; j++)
            for (int i = 0; i < 30000000; ++i);*/
        // delay_ns(30000000);
    }
    IISCON &= ~1;
    printf("play audio done\r\n");
    return 0;
}
