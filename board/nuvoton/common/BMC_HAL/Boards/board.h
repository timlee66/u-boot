/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   board.h                                                                                               */
/*            This file contains Board definitions selection, based on BOARD_NAME define                   */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _BOARD_H
#define _BOARD_H

#include <defs.h>
#include "../Common/hal_common.h"

#define __BOARD_H_FILE_NAME(name)        STRINGX(name/name.h)

/*---------------------------------------------------------------------------------------------------------*/
/* Including chip definitions                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef BOARD_NAME
#include __BOARD_H_FILE_NAME(BOARD_NAME)
#else
#error "BOARD_NAME should be defined!"
#endif




#endif /* _BOARD_H */