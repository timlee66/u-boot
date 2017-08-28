/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   clk_if.c                                                                                              */
/*            This file contains CLK module driver selector                                                */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#include "../../Common/hal_common.h"
#include "../../Chips/chip.h"

#if defined(CLK_MODULE_TYPE)
	#include __MODULE_FILE_NAME(clk, CLK_MODULE_TYPE)
#endif



