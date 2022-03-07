#ifndef _2440ADDR_HEADER_
#define _2440ADDR_HEADER_

//ram
#define BWSCON (*(volatile uint32_t*)0x48000000)
#define BANKCON6 (*(volatile uint32_t*)0x4800001C)
#define REFRESH (*(volatile uint32_t*)0x48000024)
#define BANKSIZE (*(volatile uint32_t*)0x48000028)
#define MRSRB6 (*(volatile uint32_t*)0x4800002C)
#define SDRAM_BASE 0x30000000

//gpio
#define GPBCONF (*(volatile uint32_t*)0x56000010)
#define GPBDATA (*(volatile uint32_t*)0x56000014)
#define GPBUP (*(volatile uint32_t*)0x56000018)

#define GPCCON (*(volatile uint32_t*)0x56000020)
#define GPCUP (*(volatile uint32_t*)0x56000028)

#define GPDCON (*(volatile uint32_t*)0x56000030)
#define GPDUP (*(volatile uint32_t*)0x56000038)

#define GPECON (*(volatile uint32_t*)0x56000040)
#define GPEUP (*(volatile uint32_t*)0x56000048)

#define GPGCON (*(volatile uint32_t*)0x56000060)
#define GPGUP (*(volatile uint32_t*)0x56000068)
#define GPGCONF (*(volatile uint32_t*)0x56000060)
#define GPGDATA (*(volatile uint32_t*)0x56000064)

#define GPHCON (*(volatile uint32_t*)0x56000070)
#define GPHUP (*(volatile uint32_t*)0x56000078)

//nand flash
#define NFCONF (*(volatile uint32_t*)0x4E000000)
#define NFCONT (*(volatile uint32_t*)0x4E000004)
#define NFCMMD (*(volatile uint8_t*)0x4E000008)
#define NFADDR (*(volatile uint8_t*)0x4E00000C)
#define NFDATA (*(volatile uint8_t*)0x4E000010)

#define NFMECCD0 (*(volatile uint32_t*)0x4E000014)
#define NFMECCD1 (*(volatile uint32_t*)0x4E000018)
#define NFSECCD (*(volatile uint32_t*)0x4E00001c)
#define NFSTAT (*(volatile uint8_t*)0x4E000020)
#define NFESTAT0 (*(volatile uint32_t*)0x4E000024)
#define NFESTAT1 (*(volatile uint32_t*)0x4E000028)
#define NFMECC0 (*(volatile uint32_t*)0x4E00002c)
#define NFMECC1 (*(volatile uint32_t*)0x4E000030)
#define NFSBLK (*(volatile uint32_t*)0x4E000034)
#define NFEBLK (*(volatile uint32_t*)0x4E000038)


//lcd
#define LCDCON1 (*(volatile uint32_t*)0X4D000000)
#define LCDCON2 (*(volatile uint32_t*)0X4D000004)
#define LCDCON3 (*(volatile uint32_t*)0X4D000008)
#define LCDCON4 (*(volatile uint32_t*)0X4D00000c)
#define LCDCON5 (*(volatile uint32_t*)0X4D000010)
#define LCDSADDR1 (*(volatile uint32_t*)0X4D000014)
#define LCDSADDR2 (*(volatile uint32_t*)0X4D000018)
#define LCDSADDR3 (*(volatile uint32_t*)0X4D00001c)
#define TPAL (*(volatile uint32_t*)0X4D000050)
#define LCDINTMSK  (*(volatile unsigned *)0x4d00005c)	//LCD Interrupt mask
#define TCONSEL     (*(volatile unsigned *)0x4d000060)	//LPC3600 Control --- edited by junon

//i2s
#define IISCON (*(volatile uint32_t*)0x55000000)
#define IISMOD (*(volatile uint32_t*)0x55000004)
#define IISPSR (*(volatile uint32_t*)0x55000008)
#define IISFCON (*(volatile uint32_t*)0x5500000c)
#define IISFIFO (*(volatile uint16_t*)0x55000010)

//interrupt
#define SRCPND (*(volatile uint32_t*)0x4A000000)
#define INTMOD (*(volatile uint32_t*)0x4A000004)
#define INTMSK (*(volatile uint32_t*)0x4A000008)
#define PRIORITY (*(volatile uint32_t*)0x4A00000c)
#define INTPND (*(volatile uint32_t*)0x4A000010)
#define INTOFFSET (*(volatile uint32_t*)0x4A000014)

#define EINTMASK (*(volatile uint32_t*)0x560000a4)
#define EINTPEND (*(volatile uint32_t*)0x560000a8)

//rtc
#define RTCCON (*(volatile uint32_t*)0x57000040)
#define BCDSEC (*(volatile uint8_t*)0x57000070)
#define BCDMIN (*(volatile uint8_t*)0x57000074)
#define BCDHOUR (*(volatile uint8_t*)0x57000078)
#define BCDDATE (*(volatile uint8_t*)0x5700007c)
#define BCDDAY (*(volatile uint8_t*)0x57000080)
#define BCDMON (*(volatile uint8_t*)0x57000084)
#define BCDYEAR (*(volatile uint16_t*)0x57000088)

//clock
#define CLKDIV (*(volatile uint32_t*)0x4c000014)
#define MPLLCON (*(volatile uint32_t*)0x4c000004)
#define CAMDIVN (*(volatile uint32_t*)0x4c000018)

//uart
#define ULCON0 (*(volatile uint32_t*)0x50000000)
#define UCON0 (*(volatile uint32_t*)0x50000004)
#define UFCON0 (*(volatile uint32_t*)0x50000008)
#define UMCON0 (*(volatile uint32_t*)0x5000000c)
#define UTRSTAT0 (*(volatile uint32_t*)0x50000010)
#define UMSTAT0 (*(volatile uint32_t*)0x5000001C)
#define UBRDIV0 (*(volatile uint32_t*)0x50000028)
#define UTXH0 (*(volatile uint8_t*)0x50000020)
#define URXH0 (*(volatile uint8_t*)0x50000024)

//timer
#define TCFG0 (*(volatile uint32_t*)0x51000000)
#define TCFG1 (*(volatile uint32_t*)0x51000004)
#define TCON (*(volatile uint32_t*)0x51000008)
#define TCNTB0 (*(volatile uint32_t*)0x5100000C)
#define TCMPB0 (*(volatile uint32_t*)0x51000010)


//gpio related
#define GPB5_OUT (1<<10)
#define GPB6_OUT (1<<12)
#define GPB7_OUT (1<<14)
#define GPB8_OUT (1<<16)

#define GPB5_MASK (3<<10)
#define GPB6_MASK (3<<12)
#define GPB7_MASK (3<<14)
#define GPB8_MASK (3<<16)

#define GPG0_IN (0xfffc)
#define GPG3_IN (0xff3f)
#define GPG5_IN (0xf3fc)
#define GPG6_IN (0xcfff)



#endif