// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/gic.h>
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
	unsigned int cpuid = 0 , mpidr = 0;

	__asm__ __volatile__("mrs  %0, mpidr_el1":"=r"(mpidr));

	cpuid = mpidr & 0x3;
	
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

#define DAIF_IRQ_BIT		(1<<1)	/* IRQ mask bit */
#define DAIF_FIQ_BIT		(1<<0)	/* FIQ mask bit */

int cpu_release(u32 nr, int argc, char * const argv[])
{
	u32 boot_addr =0 ;
	u32  sgir_value = 0;

	__asm __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_IRQ_BIT) : "memory");
	__asm __volatile__("msr DAIFSet, %0\n\t" : : "i" (DAIF_FIQ_BIT) : "memory");
	
	boot_addr = simple_strtoull(argv[0], NULL, 16);          /* get Address */
	sgir_value = simple_strtoull(argv[1], NULL, 10);         /* get SGI number */
	writel(boot_addr, (volatile uint32_t *)(0xf080013c));    /* Set Arbel SCRPAD fill with Address to jump */
        if ((nr < 1) || (nr > 3))
        {
		printf("Secondary core number should be (1,2,3) \n");
		return CMD_RET_USAGE;		
	}
	
	printf("\nSend SGI-%d to Core-%d and kick to Address 0x%x !\n", sgir_value, nr, boot_addr);

	__asm __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_IRQ_BIT) : "memory");
	__asm __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_FIQ_BIT) : "memory");
	
	if (nr == 1)
		sgir_value |= BIT(17);   /* CORE1 */
	if (nr == 2)
		sgir_value |= BIT(18);   /* CORE2 */
	if (nr == 3)
		sgir_value |= BIT(19);   /* CORE3 */

	sgir_value |= BIT(15);           /* Set to Group1 NSATT */
	
	
	asm volatile("dsb st");
	
	writel( sgir_value, (volatile uint32_t *)(GICD_BASE + GICD_SGIR));  /*Send SGI, Kick specific secondary core (1,2,3) */

	return 0;
}
