#include "bits.h"

bool is_bit_set(uint8_t* bits, int index)
{
    int byte_index = index / 8;
    int bit_index = index  % 8;
    return (bits[byte_index] & (1 << bit_index));
}

void set_bit(uint8_t* bits, int index, bool value)
{
    int byte_index = index / 8;
    int bit_index = index  % 8;
    if (value) {
        bits[byte_index] |= (1 << bit_index);
    } else {
        bits[byte_index] &= ~(1 << bit_index);
    }
}

int find_empty_bit(uint8_t* bits, int maxlen, int length)
{
    bool set = FALSE;
    int count = 0;
    for (int i = 0; i < maxlen; ++i) {
        set |= is_bit_set(bits, i);
        if (!set) {
            count++;
            if (count >= length) {
                return i + 1 - length;
            }
        } else {
            set = FALSE;
            count = 0;
        }
    }
    return -1;
}