#ifndef _BASE_TYPES_
#define _BASE_TYPES_

#ifndef TEST
typedef short int int16_t;
typedef unsigned short int uint16_t;
typedef long int int32_t;
typedef unsigned long int uint32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef _Bool bool;
#else
#include <stdint.h>
#endif

#define true (1)
#define false (0)
#define TRUE (1)
#define FALSE (0)
#define nullptr ((void *)0)

#endif