/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer_regs.h                                                                                          */
/*            This file contains TIMER module registers                                                    */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _TIMER_REGS_H
#define _TIMER_REGS_H

#include "../../../Chips/chip.h"



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             TIMER Register                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define TIMER_PORTS_PER_MODULE          5
#define TIMER_PORT_TO_MODULE(port)      ((port) / TIMER_PORTS_PER_MODULE)
#define TIMER_PORT_IN_MODULE(port)      ((port) % TIMER_PORTS_PER_MODULE)
#define TIMER_PORT_BASE_FUNCTION(port)  ((((port)>>1)<<5) + (((port)&1)<<2))
#define TIMER_PORT_BASE_ADDR(port)      (TIMER_BASE_ADDR(TIMER_PORT_TO_MODULE(port)) + TIMER_PORT_BASE_FUNCTION(TIMER_PORT_IN_MODULE(port)))


/*---------------------------------------------------------------------------------------------------------*/
/* Multiple per module                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define TIMER_TCSR(port)    (TIMER_PORT_BASE_ADDR(port) + 0x00), TIMER_ACCESS, 32     /* R/W Timer Control and Status Register 0 0000_0005h */
#define TIMER_TICR(port)    (TIMER_PORT_BASE_ADDR(port) + 0x08), TIMER_ACCESS, 32     /* R/W Timer Initial Control Register 0 0000_0000h */
#define TIMER_TDR(port)     (TIMER_PORT_BASE_ADDR(port) + 0x10), TIMER_ACCESS, 32     /* RO Timer Data Register 0 0000_0000h */

/*---------------------------------------------------------------------------------------------------------*/
/* Single per module                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define TIMER_TISR(port)    (TIMER_BASE_ADDR(TIMER_PORT_TO_MODULE(port))+ 0x18), TIMER_ACCESS, 32     /* R/W1C Timer Interrupt Status Register 0000_0000h */
#define TIMER_WTCR(port)    (TIMER_BASE_ADDR(TIMER_PORT_TO_MODULE(port))+ 0x1C), TIMER_ACCESS, 32     /* R/W Watchdog Timer Control Register 0000_0400h */



/*---------------------------------------------------------------------------------------------------------*/
/* Timer Control and Status Register fields                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define TCSR_PRESCALE            0, 8
#define TCSR_CACT               25, 1
#define TCSR_CRST               26, 1
#define TCSR_MODE               27, 2
#define TCSR_IE                 29, 1
#define TCSR_CEN                30, 1
#define TCSR_FREEZE_EN          31, 1

enum TCSR_MODE_type
{
	TCSR_MODE_ONESHOT   = 0x0,
	TCSR_MODE_PERIODIC  = 0x1,
	TCSR_MODE_TPGGLE    = 0x2,
};


/*---------------------------------------------------------------------------------------------------------*/
/* Timer Interrupt Status Register (TISR)fields                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define TISR_TIF0               0, 1
#define TISR_TIF1               1, 1
#define TISR_TIF2               2, 1
#define TISR_TIF3               3, 1
#define TISR_TIF4               4, 1

/*---------------------------------------------------------------------------------------------------------*/
/* Watchdog Timer Control Register (WTCR) fields                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define WTCR_WTR                 0, 1
#define WTCR_WTRE                1, 1
#define WTCR_WTRF                2, 1
#define WTCR_WTIF                3, 1
#define WTCR_WTIS                4, 2
#define WTCR_WTIE                6, 1
#define WTCR_WTE                 7, 1
#define WTCR_WTTME               8, 1
#define WTCR_FREEZE_EN           9, 1
#define WTCR_WTCLK              10, 1


#endif /* _TIMER_REGS_H */
