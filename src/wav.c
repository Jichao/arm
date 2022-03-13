#include "wav.h"
#include "common.h"
#include "kmalloc.h"
#include "../lib/libc/src/string.h"

#ifdef TEST
#include <string.h>
#endif

#pragma pack(push)
#pragma pack(2)
typedef struct tag_format_chunk {
    char id[4];
    uint32_t chunksize;
    uint16_t format;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t avg_bytes;
    uint16_t block_align;
    uint16_t bits_per_sample;
} format_chunk_t;
#pragma pack(pop)

wav_format_t *read_wav_file(const char *buffer, int len)
{
    if (len < 4) {
        return 0;
    }
    if (memcmp(buffer, "RIFF", 4) != 0) {
        printf("invalid format 1\r\n");
        return NULL;
    }

    format_chunk_t *chunk = (format_chunk_t *)(buffer + 0xc);
    if (memcmp(chunk->id, "fmt ", 4) != 0) {
        printf("invalid format 2\r\n");
        return NULL;
    }
    char *next_chunk = (char *)chunk + chunk->chunksize + 8;
    while (next_chunk < buffer + len) {
        if (memcmp(next_chunk, "data", 4) == 0) {
            wav_format_t *wav = kmalloc(sizeof(wav_format_t));
            printf("wav ptr = %p\r\n", wav);
            printf("chunk channels = %d \r\n", chunk->channels);
            wav->channels = chunk->channels;
            wav->format = chunk->format;
            wav->sample_rate = chunk->sample_rate;
            wav->bits_per_sample = chunk->bits_per_sample;
            wav->block_align = chunk->block_align;
            wav->avg_bytes_per_sec = chunk->avg_bytes;
            printf("wav header set good\r\n");
            printf("chunk size ptr = %p\r\n", next_chunk + 4);
            //wav->data_len = *(uint32_t*)(next_chunk + 4);
            wav->data_len = buff_to_u32((uint8_t*)next_chunk + 4);
            wav->data = (char*)(next_chunk + 8);
            return wav;
        }
        else {
            next_chunk += buff_to_u32((uint8_t*)next_chunk + 4) + 8;
        }
    }
    printf("invalid format 3\r\n");
    return 0;
}