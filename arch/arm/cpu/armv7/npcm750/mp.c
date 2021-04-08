// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/system.h>
#include "mp.h"
#include "cpu.h"



int is_core_valid(unsigned int core)
{
	return !!((1 << core) & cpu_mask());
}

#if 0
static int is_pos_valid(unsigned int pos)
{
	return !!((1 << pos) & cpu_pos_mask());
}
#endif

int cpu_reset(u32 nr)
{
	puts("Feature is not implemented.\n");

	return 0;
}

int cpu_disable(u32 nr)
{
	puts("Feature is not implemented.\n");

	return 0;
}


int cpu_status(u32 nr)
{
	unsigned int cpuid = 0;

	__asm__ __volatile__("mrc p15, 0, %0, c0, c0, 5":"=r"(cpuid));

    cpuid &= 0xf;
	
	if (cpuid == nr)
	{  
		printf("CPU-%d:\n", cpuid);
		return cpuid;
	}
	else
	{
		return CMD_RET_FAILURE;
	}	
}

int cpu_release(u32 nr, int argc, char * const argv[])
{
	u32 boot_addr =0 ;

	boot_addr = simple_strtoull(argv[0], NULL, 16);
	writel(boot_addr, (volatile uint32_t *)(0xf080013c));   /* Poleg SCRPAD fill with Address to jump */
        if (nr != 1)
        {
		printf("Secondary core number should be (1) \n");
		return CMD_RET_USAGE;		
	}

	asm volatile("dsb st");
	asm volatile("sev");

	return 0;
}
