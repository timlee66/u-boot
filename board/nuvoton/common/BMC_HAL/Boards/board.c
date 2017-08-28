/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   board.c                                                                                               */
/*            This file contains Board implementation selection, based on BOARD_NAME define                */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#include <defs.h>
#include "board.h"

#define __BOARD_C_FILE_NAME(name)        STRINGX(name/name.c)

/*---------------------------------------------------------------------------------------------------------*/
/* Including chip definitions                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef BOARD_NAME
#include __BOARD_C_FILE_NAME(BOARD_NAME)
#else
#error "BOARD_NAME should be defined!"
#endif

