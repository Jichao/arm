#include "mp3.h"
#include "base/common.h"
#include "mad.h"

/*
 * This is perhaps the simplest example use of the MAD high-level API.
 * Standard input is mapped into memory via mmap(), then the high-level API
 * is invoked with three callbacks: input, output, and error. The output
 * callback converts MAD's high-resolution PCM samples to 16 bits, then
 * writes them to standard output in little-endian, stereo-interleaved
 * format.
 */

int decode_mp3(unsigned char const *, unsigned long, 
    header_func_t header_callback,
    output_func_t output_callback,
    void* cb_data);
 

/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */

struct buffer {
    unsigned char const *start;
    unsigned long length;
};

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */

static enum mad_flow input(void *data, struct mad_stream *stream)
{
    // printf("input callback called\r\n");
    struct buffer *buffer = data;

    if (!buffer->length)
        return MAD_FLOW_STOP;

    mad_stream_buffer(stream, buffer->start, buffer->length);

    buffer->length = 0;

    return MAD_FLOW_CONTINUE;
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */

static enum mad_flow error(void *data, struct mad_stream *stream,
                           struct mad_frame *frame)
{
    // struct buffer *buffer = data;

    // printf("decoding error 0x%04x (%s) at byte offset %u\n", stream->error,
        //    mad_stream_errorstr(stream), stream->this_frame - buffer->start);

    /* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */

    return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */

int decode_mp3(unsigned char const * start, unsigned long length,
               header_func_t header_callback, output_func_t output_callback,
               void *cb_data)
{
    struct buffer buffer;
    struct mad_decoder decoder;
    int result;

    /* initialize our private message structure */

    buffer.start = start;
    buffer.length = length;

    /* configure input, output, and error functions */

    mad_decoder_init(&decoder, &buffer, input, 0, 0 /* filter */,
                     output_callback, error, 0 /* message */);

    /* start decoding */
    //decoder.cb_data = cb_data;
    result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

    /* release the decoder */

    mad_decoder_finish(&decoder);

    return result;
}
