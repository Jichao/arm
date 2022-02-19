#ifndef _NAND_HEADER_
#define _NAND_HEADER_

#include "common.h"

void nand_init(void);
int nand_read(uint32_t start_addr, uint32_t size, uint8_t* ram_buffer);

#endif