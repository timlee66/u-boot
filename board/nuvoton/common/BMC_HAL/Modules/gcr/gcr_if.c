/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   gcr_if.c                                                                                              */
/*            This file contains GCR module driver selector                                                */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#include "../../Common/hal_common.h"
#include "../../Chips/chip.h"

#if defined(GCR_MODULE_TYPE)
	#include __MODULE_FILE_NAME(gcr, GCR_MODULE_TYPE)
#endif



