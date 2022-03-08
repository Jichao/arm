#ifndef _BITS_HEADER_
#define _BITS_HEADER_

#include "common.h"

BOOL is_bit_set(uint8_t* bits, int index);
void set_bit(uint8_t* bits, int index, BOOL value);
int find_empty_bit(uint8_t* bits, int maxlen, int length);

#endif