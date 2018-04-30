/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nuvoton_misc.c                                                                                        */
/*            This file contains misc U-Boot interface functions                                           */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#include <config.h>
#include <common.h>
#include <sdhci.h>
#include <asm/arch/npcm750_sdhci.h>

#undef MAX
#undef MIN

#include "BMC_HAL/Chips/chip_if.h"
#include "BMC_HAL/version.h"


	DECLARE_GLOBAL_DATA_PTR;


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        reset_cpu                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine makes CPU reset                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void reset_cpu (ulong addr)
{
	TIMER_WatchdogReset(TIMER5_DEV);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        checkboard                                                                             */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Check Board Identity                                                      */
/*---------------------------------------------------------------------------------------------------------*/
int checkboard(void)
{

	char *s = getenv("serial#");
	if (s != NULL)
	{
        printf("Board serial# ");
        printf(s);
	}
	printf("\n");

	return (0);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        dram_init                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs dram initialization                                              */
/*---------------------------------------------------------------------------------------------------------*/
int dram_init (void)
{


#if 0    /* Trego - Done in Boot-Block */
	CLK_ConfigurePCIClock();            /* For DDR config */

	MC_ConfigureDDR();                  /* For DDR and debugger only perpose */
#endif

	gd->ram_size = GCR_PowerOn_GetMemorySize_limited();


	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        last_stage_init                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs last init stuff, done right before cli_loop                      */
/*---------------------------------------------------------------------------------------------------------*/

int last_stage_init(void)
{
	char memsz[32];

	sprintf(memsz, "%ldM", (long int) (((GCR_PowerOn_GetMemorySize()  - CONFIG_SYS_MEM_TOP_HIDE)/ 0x100000)));
	setenv("mem", memsz);

	nuvoton_serial_set_console_env();

	setenv("common_bootargs_dhcp", "setenv bootargs  earlycon=${earlycon} root=/dev/ram0 console=${console} mem=${mem} ramdisk_size=48000 basemac=${ethaddr} ip=dhcp");
	setenv("common_bootargs_ip",   "  setenv bootargs  earlycon=${earlycon} root=/dev/ram0 console=${console} mem=${mem} ramdisk_size=48000 basemac=${ethaddr} ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}::eth${eth_num}");


	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        interrupt_init                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs interrupt initialization                                         */
/*---------------------------------------------------------------------------------------------------------*/
int arch_interrupt_init (void)
{

	AIC_Initialize();
	timer_init();

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        print_common_info                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints board independent info                                             */
/*---------------------------------------------------------------------------------------------------------*/
void print_common_info(void)
{
	printf("HAL ver : v%s\n", BMC_HAL_VERSION_STR);
	printf("\n");
	printf("Board: %s\n", STRINGX(BOARD_NAME));
	printf("Chip : %s\n", STRINGX(CHIP_NAME));
	printf("Core : %s\n", STRINGX(CORE_TYPE));
	printf("\n");
	printf("CPU Freq:    % 4ldMHz\n", CLK_GetCPUFreq()/1000000L);
	printf("Memory Freq: % 4ldMHz\n", CLK_GetMemoryFreq()/1000000L);
	printf("SPI0 Freq:   % 4ldMHz\n", CLK_GetSPIFreq(0)/1000000L);
	printf("SPI3 Freq:   % 4ldMHz\n", CLK_GetSPIFreq(3)/1000000L);
	printf("APB1 Freq:   % 4ldMHz\n", CLK_GetAPBFreq(1)/1000000L);
	printf("APB2 Freq:   % 4ldMHz\n", CLK_GetAPBFreq(2)/1000000L);
	printf("APB3 Freq:   % 4ldMHz\n", CLK_GetAPBFreq(3)/1000000L);
	printf("APB4 Freq:   % 4ldMHz\n", CLK_GetAPBFreq(4)/1000000L);
	printf("APB5 Freq:   % 4ldMHz\n", CLK_GetAPBFreq(5)/1000000L);
	printf("CP Freq:     % 4ldMHz\n", CLK_GetCPFreq()/1000000L);
	printf("\n");

}

#ifdef CONFIG_NPCMX50_SDHCI
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        board_mmc_init                                                                             */
/*                                                                                                         */
/* Parameters:      board info                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine init mmc                                                      */
/*---------------------------------------------------------------------------------------------------------*/
int board_mmc_init(bd_t *bd)
{
	int ret = 0;

	ret = npcmx50_mmc_init();

	return ret;
}
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        misc_init_r                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  PLEASE READ WITH CARE                                                                  */
/*                                                                                                         */
/*                  This routine is serves very important purpose and shouldn't be removed from this file. */
/*                  The problem with current U-Boot implementation is that "board_eth_init" function is    */
/*                  defined as weak reference inside U-Boot code.                                          */
/*                  The archiver that makes a library out of this modules won't include "board_eth_init"   */
/*                  function because it already resolved it elswhere DESPITE the fact it was declared as   */
/*                  WEAK REFERENCE                                                                         */
/*                  This function that is actually being called by U-Boot code explicitly (without week    */
/*                  referencing) "prioritizes" this module in the function resolution process and causes   */
/*                  the archiver to take the correct (our) implementation of "board_eth_init"              */
/*---------------------------------------------------------------------------------------------------------*/
int misc_init_r (void)
{
	return 0;
}
