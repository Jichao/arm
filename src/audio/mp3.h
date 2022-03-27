#ifndef _MP3_HEADER_
#define _MP3_HEADER_

#include "mad.h"

typedef enum mad_flow (*output_func_t)(void *, struct mad_header const *, struct mad_pcm *);
typedef enum mad_flow (*header_func_t)(void *, struct mad_header const *);

int decode_mp3(unsigned char const *, unsigned long, 
    header_func_t header_callback,
    output_func_t output_callback,
    void* cb_data);
 
#endif