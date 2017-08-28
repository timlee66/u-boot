/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   fiu_if.c                                                                                              */
/*            This file contains FIU module driver selector                                                */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#include "../../Common/hal_common.h"
#include "../../Chips/chip.h"

#if defined(FIU_MODULE_TYPE)
	#include __MODULE_FILE_NAME(fiu, FIU_MODULE_TYPE)
#endif



