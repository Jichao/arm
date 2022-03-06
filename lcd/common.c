#include "common.h"

uint32_t jround(uint32_t value, uint32_t align, BOOL up)
{
    if (up) {
        return (value + align - 1) / align * align;
    } else {
        return value / align * align;
    }
}