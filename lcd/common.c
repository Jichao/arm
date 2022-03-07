#include "common.h"

uint32_t jround(uint32_t value, uint32_t align, BOOL up)
{
    if (up) {
        return (value + align - 1) / align * align;
    } else {
        return value / align * align;
    }
}
//https://stackoverflow.com/questions/13881487/should-i-worry-about-the-alignment-during-pointer-casting
//一定要注意不能够随便cast指针啊。
uint32_t buff_to_u32(unsigned char* buff)
{
    return buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
}