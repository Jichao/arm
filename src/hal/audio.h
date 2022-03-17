#ifndef _AUDIO_HEADER_
#define _AUDIO_HEADER_

#include "audio/wav.h"
#include "base/types.h"

int start_play_audio(wav_format_t* wav);
int start_play_mp3(const unsigned char* buff, int size, bool direct);

#endif