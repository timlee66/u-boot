/*
 * Copyright (c) 2015 National Instruments
 *
 * (C) Copyright 2015
 * Joe Hershberger <joe.hershberger@ni.com>
 *
 * SPDX-License-Identifier:    GPL-2.0
 */

#ifndef __MAPMEM_H
#define __MAPMEM_H

/* Define a null map_sysmem() if the architecture doesn't use it */
# ifdef CONFIG_ARCH_MAP_SYSMEM
#include <asm/io.h>
# else
static inline void *map_sysmem(phys_addr_t paddr, unsigned long len)
{
	return (void *)(uintptr_t)paddr;
}

static inline void unmap_sysmem(const void *vaddr)
{
#ifdef NPCM750
#ifndef CONFIG_SPI_FLASH_GET_RESET_ON_CORE_RESET

void SPI_Flash_Common_ExtendedAddrW(unsigned long dev_num, unsigned char HighAddr);
#include "../board/nuvoton/common/BMC_HAL/Modules/fiu/Poleg_IP/fiu_regs.h"

	/* We access area in SPI flash that might be above 16MB we need to return
	   ExtendedAddr to 0
	*/
	u32 addr = (u32)vaddr;
	if (( (addr >= SPI0CS0_BASE_ADDR) && (addr < (SPI0CS0_BASE_ADDR+SPI0_MEMORY_SIZE)) && 
		READ_REG_FIELD(FIU_DRD_CFG(0),  FIU_DRD_CFG_ADDSIZ) == 1) ||
	    ( (addr >= SPI3CS0_BASE_ADDR) && (addr < (SPI3CS0_BASE_ADDR+SPI3_MEMORY_SIZE)) && 
		READ_REG_FIELD(FIU_DRD_CFG(3),  FIU_DRD_CFG_ADDSIZ) == 1) ){
			SPI_Flash_Common_ExtendedAddrW((addr-SPI0CS0_BASE_ADDR)/FLASH_MEMORY_SIZE(0), 0);
	}
#endif
#endif
}

static inline phys_addr_t map_to_sysmem(const void *ptr)
{
	return (phys_addr_t)(uintptr_t)ptr;
}
# endif

#endif /* __MAPMEM_H */
