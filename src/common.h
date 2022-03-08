#ifndef _COMMON_HEADER_
#define _COMMON_HEADER_

#include "stdio.h"

#define HEX__(n) 0x##n##LU
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)

#define B8(d) ((unsigned char)B8__(HEX__(d)))
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
+ B8(dlsb))
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))

#ifndef TEST
typedef short int int16_t;
typedef unsigned short int uint16_t;
typedef long int int32_t;
typedef unsigned long int uint32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
typedef unsigned char uint8_t;
typedef char int8_t;
#else
#include <stdint.h>
#endif

#define SDRAM_BASE 0x30000000
#define SDRAM_CODE_BASE (SDRAM_BASE + (4096*4))
#define BOOL int
#define TRUE (1)
#define FALSE (0)

#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a > b) ? b : a)

#define offsetof(st, m) \
    ((size_t)&(((st *)0)->m))

#define kassert(cond, ...) \
    if (!cond) { \
        printf(__VA_ARGS__); \
    }

#define UDEBUG 1

#define dprintk(...) \
    if (UDEBUG == 1) \
        printf(__VA_ARGS__); \

extern uint32_t jround(uint32_t value, uint32_t align, BOOL up); 
#define jcountof(var) (sizeof(var) / sizeof(var[0]))

int roundf(float a);
uint32_t buff_to_u32(unsigned char* buff);

#include "2440addr.h"

#endif