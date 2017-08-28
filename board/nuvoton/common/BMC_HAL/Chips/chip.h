/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   chip.h                                                                                                */
/*            This file contains routing to given CHIP_NAME definitions file                               */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _CHIP_H
#define _CHIP_H

#include <defs.h>

#define __CHIP_H_FILE_NAME(name)        STRINGX(name/name.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Including chip definitions                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef CHIP_NAME
#include __CHIP_H_FILE_NAME(CHIP_NAME)
#else
#error "CHIP_NAME should be defined!"
#endif


#endif /* _CHIP_H */
