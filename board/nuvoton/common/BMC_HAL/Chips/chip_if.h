/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   chip_if.h                                                                                             */
/*            This file contains interface for the whole chip                                              */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _CHIP_IF_H_
#define _CHIP_IF_H_

#include "../Common/hal_common.h"
#include "../Common/hal_error.h"
#include "chip.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Including external definitions if needed                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef EXTERNAL_INCLUDE_FILE
#include EXTERNAL_INCLUDE_FILE
#endif

#if defined AES_MODULE_TYPE
#include "../Modules/aes/aes_if.h"
#endif
#if defined AIC_MODULE_TYPE
#include "../Modules/aic/aic_if.h"
#endif

#if defined CLK_MODULE_TYPE
#include "../Modules/clk/clk_if.h"
#endif

#if defined DES_MODULE_TYPE
#include "../Modules/des/des_if.h"
#endif

#if defined EMC_MODULE_TYPE
#include "../Modules/emc/emc_if.h"
#endif

#if defined FIU_MODULE_TYPE
#include "../Modules/fiu/fiu_if.h"
#endif

#if defined FUSE_MODULE_TYPE
#include "../Modules/fuse/fuse_if.h"
#endif

#if defined GCR_MODULE_TYPE
#include "../Modules/gcr/gcr_if.h"
#endif

#if defined GMAC_MODULE_TYPE
#include "../Modules/gmac/gmac_if.h"
#endif

#if defined MC_MODULE_TYPE
#include "../Modules/mc/mc_if.h"
#endif

#if defined TIMER_MODULE_TYPE
#include "../Modules/timer/timer_if.h"
#endif
#if defined UART_MODULE_TYPE
#include "../Modules/uart/uart_if.h"
#endif


#if defined GPIO_MODULE_TYPE
#include "../Modules/gpio/gpio_if.h"
#endif

#if defined VCD_MODULE_TYPE
#include "../Modules/vcd/vcd_if.h"
#endif

#if defined PSPI_MODULE_TYPE
#include "../Modules/pspi/pspi_if.h"
#endif

#if defined SD_MODULE_TYPE
#include "../Modules/sd/sd_if.h"
#endif

#if defined ECE_MODULE_TYPE
#include "../Modules/ece/ece_if.h"
#endif

#if defined STRP_MODULE_TYPE
#include "../Modules/strp/strp_if.h"
#endif

#endif /*_CHIP_IF_H_ */
