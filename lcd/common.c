#include "common.h"

uint32_t jround(uint32_t value, uint32_t align, BOOL up)
{
    if (up) {
        return (value + align - 1) / align * align;
    } else {
        return value / align * align;
    }
}

uint32_t buff_to_u32(unsigned char* buff)
{
    return buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
}