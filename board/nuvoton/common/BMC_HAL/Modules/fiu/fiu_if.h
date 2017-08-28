/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   fiu_if.h                                                                                              */
/*            This file contains FIU interface                                                             */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _FIU_IF_H
#define _FIU_IF_H

#include "defs.h"
#include "../../Common/hal_error.h"

/*---------------------------------------------------------------------------------------------------------*/
/* FIU Modules                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum FIU_MODULE_T_tag
{
	FIU_MODULE_0 = 0,
	FIU_MODULE_1 = 1,
	FIU_MODULE_2 = 2,
	FIU_MODULE_3 = 3,
	FIU_MODULE_X = 4
} FIU_MODULE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_Init                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  num_device       - number of flash devices connected to the chip                       */
/*                  max_dev_size     - Size of the biggest device connected                                */
/*                  total_flash_size - The total flash size in Kbytes                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configs the FUI                                                           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FIU_Init(UINT num_device, UINT32 max_dev_size, UINT32 total_flash_size);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_uma_read                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                device           - Select the flash device (0 or 1) to be accessed                       */
/*                transaction_code - Specify the SPI UMA transaction code                                  */
/*                address          - Location on the flash , in the flash address space                    */
/*                address_size     - if TRUE, 3 bytes address, to be placed in UMA_AB0-2                   */
/*                                   else (FALSE), no address for this SPI UMA transaction                 */
/*                data             - a pointer to a data buffer to hold the read data.                     */
/*                data_size        - buffer size. Legal sizes are 1,2,3,4                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Read up to 4 bytes from the flash. using the FIU User Mode Access (UMA)   */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FIU_UMA_Read(UINT device, UINT8 transaction_code, UINT32 address, BOOLEAN address_size, UINT8 * data, UINT32 data_size);




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_uma_write                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              device           - Select the flash device (0 or 1) to be accessed                         */
/*              transaction_code - Specify the SPI UMA transaction code                                    */
/*              address          - Location on the flash, in the flash address space                       */
/*              address_size     - if TRUE, 3 bytes address, to be placed in UMA_AB0-2                     */
/*                                 else (FALSE), no address for this SPI UMA transaction                   */
/*              data             - a pointer to a data buffer (buffer of bytes)                            */
/*              data_size        - data buffer size in bytes. Legal sizes are 0,1,2,3,4                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*           This routine write up to 4 bytes to the flash using the FIU User Mode Access (UMA)            */
/*           which allows the core an indirect access to the flash, bypassing FIU flash write              */
/*           protection.                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FIU_UMA_Write(UINT device, UINT8 transaction_code, UINT32 address, BOOLEAN address_size, UINT8 * data, UINT32 data_size);





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_manual_write                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              device           - Select the flash device (0 or 1) to be accessed                         */
/*              transaction_code - Specify the SPI UMA transaction code                                    */
/*              address          - Location on the flash, in the flash address space                       */
/*              data             - a pointer to a data buffer (buffer of bytes)                            */
/*              data_size        - data buffer size in bytes. Legal sizes are 0-256                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine "manual" page programming without using UMA.                              */
/*                  The data can be programmed upto the size of the whole page (256 bytes) in a single     */
/*                  SPI transaction                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FIU_ManualWrite(UINT device, UINT8 transaction_code, UINT32 address, UINT8 * data, UINT32 data_size);

#endif /*_FIU_IF_H */


