/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   sd_if.h                                                                                               */
/*            This file contains SD driver interface                                                       */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef SD_IF_H
#define SD_IF_H

#include "defs.h"
#include "../timer/timer_if.h"
#include "../../Common/hal_error.h"

/*---------------------------------------------------------------------------------------------------------*/
/* SD device definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	SD1_DEV = 0,
	SD2_DEV = 1,
} SD_DEV_NUM_T;

typedef enum
{
	SD_DEV_SD  = 0,
	SD_DEV_MMC = 1,
	SD_DEV_MMC8 = 2,
} SD_DEV_TYPE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* SD block definitions                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define SD_SECTOR_SIZE          512

/*---------------------------------------------------------------------------------------------------------*/
/* SD clock configurations definitions                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

/* Clock frequency type */
typedef enum
{
	SD_CLK_400_KHZ = (400 * _1KHz_),    /* 400  KHz */
	SD_CLK_20_MHZ  = (20  * _1MHz_),    /* 20   MHz */
	SD_CLK_25_MHZ  = (25  * _1MHz_),    /* 25   MHz */
	SD_CLK_50_MHZ  = (50  * _1MHz_),    /* 50   MHz */
} SD_CLK_T;


/*---------------------------------------------------------------------------------------------------------*/
/* SD function definitions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     SD_Init                                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               devNum   - [in], SD device number                                                         */
/*               devType  - [in], SD1 device type (SD or MMC)                                              */
/*               timerNum - [in], Timer device number                                                      */
/*                                                                                                         */
/* Returns:      Flash Card Error Code                                                                     */
/* Side effects: None                                                                                      */
/* Description:                                                                                            */
/*               This routine initializes the SD HW module and SW driver without                           */
/*               accessing the card (no card initialization)                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_Init (SD_DEV_NUM_T devNum, SD_DEV_TYPE_T devType, TIMER_DEV_T timerNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     SD_InitializeCard                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               devNum   - [in], SD device number                                                         */
/*                                                                                                         */
/* Returns:      BMC HAL Error Code                                                                        */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*               This routine performs SD/MMC card identification process,                                 */
/*               as defined by the SD and MMC System Specifications, chapter 4.2.3.                        */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_InitializeCard (SD_DEV_NUM_T devNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SD_ReadDataBuf                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - [in], SD device number                                                        */
/*                                                                                                         */
/* Returns:         SD data buffer contents                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads SD Buffer (32-bit) Data Port.                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 SD_ReadDataBuf (SD_DEV_NUM_T devNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SD_BufferReadEnabled                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - [in], SD device number                                                        */
/*                                                                                                         */
/* Returns:         TRUE if read buffer is enabled, FALSE otherwise                                        */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks whether SD buffer read is enabled.                                 */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN SD_BufferReadEnabled (SD_DEV_NUM_T devNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SD_EmptyFifoBuffer                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -   [in], SD device number                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Reads and discards the data from fifo buffer until no more data is available           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_EmptyFifoBuffer(SD_DEV_NUM_T devNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     SD_ReadDataBlocks                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               devNum     - [in], SD device number                                                       */
/*               deviceAddr - [in], data address in device to read from                                    */
/*               memoryAddr - [out], buffer to read data into                                              */
/*               count      - [in], number of bytes to read                                                */
/*               bUseDMA    - [in], TRUE to use DMA, FALSE to use uC                                       */
/*                                                                                                         */
/* Returns:      Flash Card Error Code                                                                     */
/* Side effects: If 'bUseDMA' is TRUE, SD DMA ia used.                                                     */
/* Description:                                                                                            */
/*               This routine reads contiguous data blocks from the SD card.                               */
/*               If the DMA is in use, READ_MULTIPLE_BLOCK command is initiated by the micro-controller,   */
/*               transaction is stopped automatically by DMA, and data is read into 'memoryAddr'.          */
/*               If DMA is not used, the uC reads the data into 'memoryAddr'.                              */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_ReadDataBlocks (SD_DEV_NUM_T devNum, UINT32 SrcAddr, UINT8* DstAddr, UINT32 cnt, BOOLEAN bUseDMA);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     SD_WriteDataBlocks                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               devNum     - [in], SD device number                                                       */
/*               deviceAddr - [in], data address in device to read from                                    */
/*               memoryAddr - [out], buffer to read data into                                              */
/*               count      - [in], number of bytes to read                                                */
/*               bUseDMA    - [in], TRUE to use DMA, FALSE to use uC                                       */
/*                                                                                                         */
/* Returns:      Flash Card Error Code                                                                     */
/* Side effects: If 'bUseDMA' is TRUE, SD DMA ia used.                                                     */
/* Description:                                                                                            */
/*               This routine writes contiguous data blocks to the SD card.                                */
/*               If the DMA is in use, WRITE_MULTIPLE_BLOCK command is initiated by the micro-controller,  */
/*               transaction is stopped automatically by DMA, data is written into the SD card memory.     */
/*               If DMA is not used, the uC writes the data from 'memoryAddr'.                             */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_WriteDataBlocks (SD_DEV_NUM_T devNum, UINT32 SrcAddr, UINT8* DstAddr, UINT32 cnt, BOOLEAN bUseDMA);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     SD_SetClock                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               devNum   - [in], SD device number                                                         */
/*               clk      - [in], Target clock frequency (in Hz units)                                     */
/*                                                                                                         */
/* Returns:      Flash Card Error Code                                                                     */
/* Side effects: None                                                                                      */
/* Description:                                                                                            */
/*               This routine sets the clock frequency by adjusting the                                    */
/*               SDCLK Frequency Select using the Base Clock Frequency For SD Clock.                       */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_SetClock (SD_DEV_NUM_T devNum, SD_CLK_T clk);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SD_CardPowerOn                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -  SD device number                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Enables the clock to the card and powers on the card if implemented on the board       */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_CardPowerOn(SD_DEV_NUM_T devNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SD_CardPowerOff                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -  SD device number                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Disables the clock to the card and powers off the card if implemented on the board     */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_CardPowerOff(SD_DEV_NUM_T devNum);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     SD_Isr                                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               devNum   - [in], SD device number                                                         */
/*                                                                                                         */
/* Returns:      BMC HAL Error Code                                                                        */
/* Side effects: None                                                                                      */
/* Description:                                                                                            */
/*               SD interrupt service routine. The handler covers the following sources:                   */
/*                1) Normal Interrupt Status.                                                              */
/*                2) Error Interrupt Status.                                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SD_Isr (UINT32 devNum);

#endif /* SD_IF_H */

