#ifndef _AUDIO_HEADER_
#define _AUDIO_HEADER_

#include "wav.h"

int start_play_audio(wav_format_t* wav);
int start_play_mp3(unsigned char* buff, int size);

#endif