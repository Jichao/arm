#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_

#include "base/types.h"
#include "base/macros.h"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define UDEBUG 0

#define kassert(cond, ...) \
    if (!cond) { \
        printf("\033[31m"); \
        printf("%s(%d): %s :", __FILE__, __LINE__, __FUNCTION__); \
        printf(__VA_ARGS__); \
        printf("\033[0m"); \
    }

#define dprintk(...) \
    if (UDEBUG == 1) { \
        printf("%s(%d): %s :", __FILE__, __LINE__, __FUNCTION__); \
        printf(__VA_ARGS__); \
    }

#define eprintk(...) \
    printf("\033[31m"); \
    printf("%s(%d): %s :", __FILE__, __LINE__, __FUNCTION__); \
    printf(__VA_ARGS__); \
    printf("\033[0m"); 

#define printk(...) \
        printf("%s(%d): %s :", __FILE__, __LINE__, __FUNCTION__); \
        printf(__VA_ARGS__); \

uint32_t jround(uint32_t value, uint32_t align, bool up); 
int roundf(float a);
uint32_t buff_to_u32(unsigned char* buff);
void abort(void);
void dump_current_context(void);

#endif
