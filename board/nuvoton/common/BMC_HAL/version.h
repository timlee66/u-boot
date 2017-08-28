/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   version.h                                                                                             */
/*            This file contains version number of BMC HAL                                                 */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#define BMC_HAL_MAJOR   0
#define BMC_HAL_MINOR   3
#define BMC_HAL_REV     2

#define BMC_HAL_VERSION         MAKE32(MAKE16(BMC_HAL_REV, BMC_HAL_MINOR), MAKE16(BMC_HAL_MAJOR, 0x00))
#define BMC_HAL_VERSION_STR     STRINGX(BMC_HAL_MAJOR) "." STRINGX(BMC_HAL_MINOR) "." STRINGX(BMC_HAL_REV)


