/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   pspi_if.h                                                                                             */
/*            This file contains Peripheral SPI interface                                                  */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _PSPI_IF_H
#define _PSPI_IF_H

#include "../../Common/hal_error.h"
#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/* PSPI Devices                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	PSPI1_DEV = 0,
	PSPI2_DEV = 1,
} PSPI_DEV_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Data transfer modes                                                                                     */
/* Either output data is written on rising edge and input data read on falling edge or vise versa          */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	PSPI_WRITE_RISING_EDGE,
	PSPI_WRITE_FALLING_EDGE,
} PSPI_TRANSFER_MODE;



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PSPI_Init                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum      - number of PSPI device                                                    */
/*                  mode        - Data shifting mode                                                       */
/*                  clockFreq   - Data shifting clock                                                      */
/*                                                                                                         */
/* Returns:         Returns HAL_OK on success or ERROR CODE on failure                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs PSPI module initialization                                       */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS PSPI_Init(PSPI_DEV_T devNum, PSPI_TRANSFER_MODE mode, UINT32 clockFreq);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PSPI_Transfer                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum      - number of PSPI device                                                    */
/*                  csGpio      - GPIO number of the Chip Select                                           */
/*                  writeBuf    - Buffer for sending                                                       */
/*                  readBuf     - Buffer for receiving                                                     */
/*                  size        - Buffers size (both buffers must have the same size                       */
/*                                                                                                         */
/* Returns:         HAL_OK on success or HAL_ERROR_* or error                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SPI tranfer.                                                     */
/*                  Each clock cycle 1 bit from writeBuf is written out and 1 bit is read into the         */
/*                  readBuf.                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS PSPI_Transfer (PSPI_DEV_T devNum, UINT csGpio, UINT8* writeBuf, UINT8* readBuf, UINT size);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PSPI_WriteRead                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum      - number of PSPI device                                                    */
/*                  csGpio      - GPIO number of the Chip Select                                           */
/*                  writeBuf    - Buffer for sending                                                       */
/*                  writeSize   - sending buffer size                                                      */
/*                  readBuf     - Buffer for reading                                                       */
/*                  readSize    - sending buffer size                                                      */
/*                                                                                                         */
/* Returns:         HAL_OK on success or HAL_ERROR_* or error                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine first writes a data on the SPI bus (from writeBuf) and then reads the     */
/*                  data from the SPI bus into the readBuf                                                 */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS PSPI_WriteRead(PSPI_DEV_T devNum, UINT csGpio, UINT8* writeBuf,  UINT writeSize, UINT8* readBuf, UINT readSize);



#endif /*_PSPI_IF_H */

