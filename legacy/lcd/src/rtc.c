#include "rtc.h"
#include "common.h"

#define RTCCON (*(volatile uint32_t*)0x57000040)
#define BCDSEC (*(volatile uint8_t*)0x57000070)
#define BCDMIN (*(volatile uint8_t*)0x57000074)
#define BCDHOUR (*(volatile uint8_t*)0x57000078)
#define BCDDATE (*(volatile uint8_t*)0x5700007c)
#define BCDDAY (*(volatile uint8_t*)0x57000080)
#define BCDMON (*(volatile uint8_t*)0x57000084)
#define BCDYEAR (*(volatile uint16_t*)0x57000088)

#define SEC_PER_MIN 60
#define SEC_PER_HOUR (60 * 60)
#define SEC_PER_DAY (60 * 60) * 24

void reset_time(void)
{
    RTCCON = 0x1;
    BCDYEAR = 22;
    BCDMON = 2;
    BCDDATE = (2 << 4) + 6;
    BCDDAY = 6;
    BCDHOUR = (2 << 4) + 2;
    BCDMIN = (4 << 4) + 4;
    BCDSEC = 0;
    RTCCON = 0;
}

void output_time(void)
{
    RTCCON = 0x1;
    uint8_t sec,min,hour,day,month;
    unsigned int year;
    sec = BCDSEC;
    sec = ((sec >> 4) & 0x7) * 10 + (sec & 0xf);
    min = BCDMIN;
    min = ((min >> 4) & 0x7) * 10 + (min & 0xf);
    hour = BCDHOUR;
    hour = ((hour >> 4) & 0x3) * 10 + (hour & 0xf);
    day = BCDDATE;
    day = ((day >> 4) & 0x2) * 10 + (day & 0xf);
    month = BCDMON;
    month = ((month >> 4) & 0x1) * 10 + (month & 0xf);
    year = 2000 + BCDYEAR;
    RTCCON = 0;
    printf("\n\rcurrent time: %04d-%02d-%02d %02d:%02d:%02d\n\r", year, month, day, hour, min, sec);
}

uint32_t get_sec_count(void)
{
    RTCCON = 0x1;
    uint8_t sec,min,hour,day,month;
    unsigned int year;
    sec = BCDSEC;
    sec = ((sec >> 4) & 0x7) * 10 + (sec & 0xf);
    min = BCDMIN;
    min = ((min >> 4) & 0x7) * 10 + (min & 0xf);
    hour = BCDHOUR;
    hour = ((hour >> 4) & 0x3) * 10 + (hour & 0xf);
    day = BCDDATE;
    day = ((day >> 4) & 0x2) * 10 + (day & 0xf);
    month = BCDMON;
    month = ((month >> 4) & 0x1) * 10 + (month & 0xf);
    year = 2000 + BCDYEAR;
    RTCCON = 0;    
    return year * 365 * 12 * 30 * SEC_PER_DAY + month * 30 * SEC_PER_DAY + day * SEC_PER_DAY + hour * SEC_PER_HOUR + sec;
}