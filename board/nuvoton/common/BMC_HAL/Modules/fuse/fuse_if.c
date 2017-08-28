/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   fuse_if.c                                                                                             */
/*            This file contains FUSE module driver selector                                               */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#include "../../Common/hal_common.h"
#include "../../Chips/chip.h"

#if defined(FUSE_MODULE_TYPE)
	#include __MODULE_FILE_NAME(fuse, FUSE_MODULE_TYPE)
#endif



