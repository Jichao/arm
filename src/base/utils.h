#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_

#include "base/types.h"
#include "base/macros.h"

#define UDEBUG 1

#define kassert(cond, ...) \
    if (!cond) { \
        printf(__VA_ARGS__); \
    }

#define dprintk(...) \
    if (UDEBUG == 1) { \
        printf("%s(%d): %s :", __FILE__, __LINE__, __FUNCTION__); \
        printf(__VA_ARGS__); \
    }

#define printk(...) \
        printf("%s(%d): %s :", __FILE__, __LINE__, __FUNCTION__); \
        printf(__VA_ARGS__); \

uint32_t jround(uint32_t value, uint32_t align, bool up); 
int roundf(float a);
uint32_t buff_to_u32(unsigned char* buff);
void abort(void);
void dump_current_context(void);

#endif
