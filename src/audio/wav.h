#ifndef _WAV_HEADEER_
#define _WAV_HEADEER_

typedef struct tag_wav_format {
    int format;
    int channels;
    int sample_rate;
    int bits_per_sample;
    int avg_bytes_per_sec;
    int block_align;
    int data_len;
    char* data;
} wav_format_t;

wav_format_t *read_wav_file(const char *buffer, int len);

#endif