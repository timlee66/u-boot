/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer_if.c                                                                                            */
/*            This file contains TIMER module driver selector                                              */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#include "../../Common/hal_common.h"
#include "../../Chips/chip.h"

#if defined(TIMER_MODULE_TYPE)
	#include __MODULE_FILE_NAME(timer, TIMER_MODULE_TYPE)
#endif




