/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nuvoton_aic.c                                                                                         */
/*            This file contains implementation of AIC module for U-Boot                                   */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#include "config.h"
#undef MAX
#undef MIN

#include "BMC_HAL/Chips/chip_if.h"



/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Handler Table                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        do_irq                                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is the main IRQ handler                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef CONFIG_USE_IRQ
void do_irq(void)
{
	AIC_Isr();
}
#endif


