/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   PolegSVB.h                                                                                           */
/*            This file contains PolegSVB definitions                                                     */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _POLEG_SVB_H_
#define _POLEG_SVB_H_

/*---------------------------------------------------------------------------------------------------------*/
/*                                             Chip interface                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define CHIP_NAME   npcm750
#include "../../Chips/chip_if.h"

#ifdef __LINUX_KERNEL_ONLY__

#undef MFT_MODULE_TYPE
/*#undef GCR_MODULE_TYPE */
/*#undef UART_MODULE_TYPE */
/*#undef TIMER_MODULE_TYPE */
#undef SMB_MODULE_TYPE
#undef KCS_MODULE_TYPE
#undef ADC_MODULE_TYPE
#undef PWM_MODULE_TYPE
/*#undef FIU_MODULE_TYPE */
/*#undef CLK_MODULE_TYPE */
#undef SHM_MODULE_TYPE
/*#undef AIC_MODULE_TYPE */
#undef EMC_MODULE_TYPE
#undef GMAC_MODULE_TYPE
#undef MC_MODULE_TYPE
#undef SD_MODULE_TYPE
#undef AES_MODULE_TYPE
#undef DES_MODULE_TYPE
/*#undef STRP_MODULE_TYPE */
/*#undef FUSE_MODULE_TYPE */
/*#undef VCD_MODULE_TYPE */
#undef GFX_MODULE_TYPE
/*#undef ECE_MODULE_TYPE */
#undef VDM_MODULE_TYPE
/*#undef GPIO_MODULE_TYPE */
#undef PSPI_MODULE_TYPE
#undef USB_MODULE_TYPE

#endif


/*---------------------------------------------------------------------------------------------------------*/
/*                                       Board dependent parameters                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*Trego - Add board specific */

/*#define POLEG_DRB_HW         1 */



/*---------------------------------------------------------------------------------------------------------*/
/* PHY assignment per module                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define BOARD_ETH0_PHY_OPS              BCM5221_Ops        /* EMC1 */
#define BOARD_ETH1_PHY_OPS              BCM5221_Ops        /* EMC2 */

#define ETH0_MAC_ADDRESS {0x00,0x00,0xF7,0xA0,0x00,0x45}   /* EMC1 */
#define ETH1_MAC_ADDRESS {0x00,0x00,0xF7,0xA0,0x00,0x46}   /* EMC2 */
#define ETH2_MAC_ADDRESS {0x00,0x00,0xF7,0xA0,0x00,0x47}   /* GMAC1 */
#define ETH3_MAC_ADDRESS {0x00,0x00,0xF7,0xA0,0x00,0x48}   /* GMAC2 */


/*---------------------------------------------------------------------------------------------------------*/
/* SPI Flash                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

/* define CONFIG_SPI_FLASH_GET_RESET_ON_CORE_RESET only if on any core reset forces the SPI device
   to get to reset too. It depends on system schematics
*/
//#define CONFIG_SPI_FLASH_GET_RESET_ON_CORE_RESET

#define CONFIG_FLASH_BASE               FLASH_BASE_ADDR(0)

#define CONFIG_SYS_FLASH_SIZE		    (_64MB_)

#define CONFIG_SYS_MAX_FLASH_SECT       (CONFIG_SYS_FLASH_SIZE / _4KB_)

#define CONFIG_SPI3_ENABLE              /* add this if you use SPI3 */

#ifdef CONFIG_SPI3_ENABLE
#define CONFIG_SYS_MAX_FLASH_BANKS      4
#else
#define CONFIG_SYS_MAX_FLASH_BANKS      2
#endif

#define BOOTER_VER_ADDR                0x800002F4

#define CONFIG_IMAGE_FORMAT_LEGACY


/*---------------------------------------------------------------------------------------------------------*/
/* EMC configuration                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*#define BOARD_EMC_USING_GMAC_MDIO       1 */

/*---------------------------------------------------------------------------------------------------------*/
/*                                          Peripherals interface                                          */
/*---------------------------------------------------------------------------------------------------------*/
#include "../../Peripherals/spi_flash/spi_flash.h"



#endif /*_POLEG_SVB_H_ */

