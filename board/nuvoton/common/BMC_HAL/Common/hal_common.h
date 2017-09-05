/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hal_common.h                                                                                          */
/*            This file contains common HAL definitions                                                    */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _HAL_COMMON_H_
#define _HAL_COMMON_H_

#define __EXTERNAL_FILE(name)           STRINGX(name)
#define __MODULE_FILE_NAME(moduleType, moduleVer)        STRINGX(../moduleType/moduleVer/moduleType##_drv.c)


/*---------------------------------------------------------------------------------------------------------*/
/* Including defs.h                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Including external definitions if needed                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef EXTERNAL_INCLUDE_FILE
#include EXTERNAL_INCLUDE_FILE
#endif



/*---------------------------------------------------------------------------------------------------------*/
/* Common definitions for the HAL                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef VIRT_TO_PHYS
#define VIRT_TO_PHYS(x)     (x)
#endif

#ifndef PHYS_TO_VIRT
#define PHYS_TO_VIRT(x)     (x)
#endif

#ifndef DCACHE_INV_RANGE
#define DCACHE_INV_RANGE(start, end)
#endif

#ifndef DCACHE_CLEAN_RANGE
#define DCACHE_CLEAN_RANGE(start, end)
#endif

#ifndef DCACHE_FLUSH_RANGE
#define DCACHE_FLUSH_RANGE(start, end)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* HAL Sleep macro                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef HAL_USLEEP

#define HAL_USLEEP(timer, usec)                 \
        {                                       \
            TIMER_StartOneShot(timer, usec);    \
            while(TIMER_OneShotRunning(timer)); \
        }
#endif

#endif /*_HAL_COMMON_H_ */

