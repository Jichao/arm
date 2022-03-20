#ifndef _NAND_HEADER_
#define _NAND_HEADER_

#include "base/common.h"

void nand_init(void);
int nand_to_ram(uint32_t start_addr, uint32_t size, uint8_t* ram_addr);
uint32_t nand_read_dev_id(void);

#endif