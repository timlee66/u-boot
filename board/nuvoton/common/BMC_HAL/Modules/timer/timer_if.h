/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer_if.h                                                                                           */
/*            This file contains TIMER module interface                                                    */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _TIMER_IF_H
#define _TIMER_IF_H

#include "../aic/aic_if.h"
#include "../../Common/hal_error.h"
#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Timer definitions                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	/* TIMER_MODULE_0 : */
	TIMER0_DEV  = 0,
	TIMER1_DEV  = 1,
	TIMER2_DEV  = 2,
	TIMER3_DEV  = 3,
	TIMER4_DEV  = 4,

	/* TIMER_MODULE_1 : */
	TIMER5_DEV  = 5,
	TIMER6_DEV  = 6,
	TIMER7_DEV  = 7,
	TIMER8_DEV  = 8,
	TIMER9_DEV  = 9,

	/* TIMER_MODULE_2 : */
	TIMER10_DEV  = 10,
	TIMER11_DEV  = 11,
	TIMER12_DEV  = 12,
	TIMER13_DEV  = 13,
	TIMER14_DEV  = 14,
} TIMER_DEV_T;

UINT32 TIMER_GetCurrVal(TIMER_DEV_T timerNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_StartPeriodic                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum        - Timer number                                                         */
/*                  ticksPerSec     - Timer frequency in hertz                                             */
/*                  tickHanlder     - Handler for Timer tick event. If NULL is given, internal event       */
/*                                    handler is used                                                      */
/*                  tickHandlerArg  - Argument to the handler                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialization of the timer to perform periodic event.                    */
/*                  If 'tickHandler' is given it is called every 'tickPerSec' tick.                        */
/*                  Otherwise internal handler is increasing internal tick counter for the given timer.    */
/*                  Its value can be read using TIMER_GetTick function                                     */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS TIMER_StartPeriodic(TIMER_DEV_T timerNum, UINT32 ticksPerSec, AIC_Isr_T tickHanlder, void* tickHandlerArg);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_stopPeriodic                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum -                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine stops the timer started by startPeriodic routine                          */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS TIMER_StopPeriodic(TIMER_DEV_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_startOneShot                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  microSec -    number of micro seconds the timer will count (to zero)                   */
/*                  timerNum -    the timer number to use                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Activate timer in OneShot mode, no use of interrupts                                   */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS TIMER_StartOneShot(TIMER_DEV_T timerNum, UINT32 microSec);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_oneShotRunning                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum -  number of timer to check                                                   */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Checks whether the specified timer is still running (counting)                         */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN TIMER_OneShotRunning(TIMER_DEV_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_reset                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs TIMER reset                                                      */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS TIMER_Reset (TIMER_DEV_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_getTick                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns TIMER value                                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 TIMER_GetTick(TIMER_DEV_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_getHWTick                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum -                                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine return the content of the HW ticking register                             */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 TIMER_GetHWTick(TIMER_DEV_T timerNum);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_Isr                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum - timer number                                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is TIMER IRQ handler                                                      */
/*                  It should be calle upon TIMER Interrupt event for all timers                           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS TIMER_Isr(UINT32 timerNum);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_watchdogRest                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timerNum - timer number                                                                */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs watchdog reset                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void TIMER_WatchdogReset(UINT32 timerNum);




#endif /* _TIMER_IF_H */

