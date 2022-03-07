#ifndef _AUDIO_HEADER_
#define _AUDIO_HEADER_

typedef struct tag_wav_format {
    int format;
    int channels;
    int sample_rate;
    int bits_per_sample;
    int avg_bytes_per_sec;
    int block_align;
    char* data;
} wav_format_t;

int start_play_audio(wav_format_t* wav);

#endif