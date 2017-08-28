/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nuvoton_timer.c                                                                                       */
/*            This file contains timer module implementation                                               */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

#include <config.h>
#include <common.h>

#undef MAX
#undef MIN

#include "BMC_HAL/Chips/chip_if.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        timer_init                                                                             */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs timer initialization                                             */
/*---------------------------------------------------------------------------------------------------------*/
int timer_init (void)
{
	TIMER_StartPeriodic(TIMER0_DEV, CONFIG_SYS_HZ, NULL, NULL);

	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        reset_timer                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs timer reset                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void reset_timer (void)
{
	TIMER_Reset(TIMER0_DEV);
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        get_timer                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns timer value normalized with base                                  */
/*---------------------------------------------------------------------------------------------------------*/
ulong get_timer (ulong base)
{
	return TIMER_GetTick(TIMER0_DEV) - base;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        get_ticks                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns number of ticks                                                   */
/*---------------------------------------------------------------------------------------------------------*/
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        udelay                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  usec -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine delay x useconds AND perserve advance timstamp value                      */
/*---------------------------------------------------------------------------------------------------------*/
void __udelay (unsigned long usec)
{
	CLK_Delay_MicroSec((UINT32)usec);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        get_tbclk                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  usec -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the number of timer ticks per second                              */
/*---------------------------------------------------------------------------------------------------------*/
ulong get_tbclk (void)
{
	return CONFIG_SYS_HZ;
}






/*---------------------------------------------------------------------------------------------------------*/
/* Tick Tock function: get elapsed time using the 25MHz HW internal counter                                */
/*---------------------------------------------------------------------------------------------------------*/

UINT32          gTimeTick[2];

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Tick                                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  time tick  (start counting time)                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void Tick (void)
{
	CLK_GetTimeStamp(gTimeTick);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Tock                                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  time tock  (stop and print time since the Tick)                                        */
/*---------------------------------------------------------------------------------------------------------*/
void Tock (void)
{
	ulong          time_tock = 0;

	time_tock = CLK_Delay_Since(0, gTimeTick);
	printf(" =>UBOOT last command run for %ld us.\n", time_tock);
}




