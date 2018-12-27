/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   PolegSVB_init.c                                                                                      */
/*            This file contains WPCM450 initialization                                                    */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

#include <common.h>

#undef MAX
#undef MIN

#include "../common/BMC_HAL/Chips/chip_if.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Using external timer_init function                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
extern int  timer_init (void);
extern void print_common_info(void);


	DECLARE_GLOBAL_DATA_PTR;


/*---------------------------------------------------------------------------------------------------------*/
/* Miscellaneous platform dependent initialisations                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int board_init (void)
{
	gd->bd->bi_arch_number = MACH_TYPE_NPCM750;
	gd->bd->bi_boot_params = CONFIG_TAGS_LOCATION;

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        print_cpuinfo                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints CPU and Board info                                                 */
/*---------------------------------------------------------------------------------------------------------*/
int print_cpuinfo (void)
{
	print_common_info();

	#define ROMCODE_VER     *((PTR32)(ROMCODE_VER_ADDR))
	#define BOOTER_VER      *((PTR32)(BOOTER_VER_ADDR))
  #define SPSWC           0x004
  #define MFSEL1          0x00c
  #define INTCR           0x03c

	printf(KMAG "RomCode   : v%02x.%02x.%02x.%02x\n", MSB0(ROMCODE_VER), MSB1(ROMCODE_VER), MSB2(ROMCODE_VER), MSB3(ROMCODE_VER));
	printf("BootBlock : v%02x.%02x.%02x.%02x\n", MSB0(BOOTER_VER),  MSB1(BOOTER_VER),  MSB2(BOOTER_VER),  MSB3(BOOTER_VER));

	printf("\n" KNRM);

	printf("PolegSVB Power-On Straps = 0x%x\n",(unsigned int)*((PTR32)(GCR_BASE_ADDR + 0x004)));

  unsigned int spmod, hsl1sel;
  spmod = 0x06;
  hsl1sel = 0x400;

  *((volatile uint32_t *)(GCR_BASE_ADDR + SPSWC)) |= spmod;
  *((volatile uint32_t *)(GCR_BASE_ADDR + MFSEL1)) |= hsl1sel;
  *((volatile uint32_t *)(GCR_BASE_ADDR + INTCR)) |= 0x4700;
  *((volatile uint32_t *)(CLK_BASE_ADDR + 0x004)) |= 0x01E18F49;




	if (STRP_SecureBoot_On())
	{
        printf("Security: ENABLED, running in secure mode\n");
	}
	else
	{
        printf("Security: DISABLED, running in non-secure mode\n");
	}

	return 0;
}


