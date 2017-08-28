/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   spi_flash.h                                                                                           */
/*            This file contains interface to SPI Flash driver                                             */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _FLAHS_SPI_H
#define _FLAHS_SPI_H

#include "defs.h"
#include "../../Common/hal_error.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Flash Device Class                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct flash_t
{
	UINT32          mf_id;
	UINT32          capacity_id;
	UINT8*          name;
	UINT32          chip_size;
	UINT32          sector_size;
	UINT32          page_size;
	UINT32          startAddr;

	void            (*FlashPageProg)    (UINT32 devID, UINT32 destAddr, UINT8* data, UINT32 size);
	void            (*FlashSectorErase) (UINT32 devID, UINT32 destAddr);
	void            (*FlashReadStatus)  (UINT32 devID, UINT8 *statusReg);
	void            (*FlashBulkErase)   (UINT32 devID);

}flash_t;


/*---------------------------------------------------------------------------------------------------------*/
/* API Variables                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
extern flash_t SPI_Flash_Devices[CONFIG_SYS_MAX_FLASH_BANKS];
extern UINT32  SPI_Flash_TotalSize;


/*---------------------------------------------------------------------------------------------------------*/
/*                                              API Functions                                              */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Init                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  flashBaseAddress    - base address on the flash                                        */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SPI flash initialization                                         */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SPI_Flash_Init(UINT32 flashBaseAddress);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Write                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  destAddr    - destination address on the flash                                         */
/*                  data        - source data ptr                                                          */
/*                  size        - source data size                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs logical write to the flash including smart erase sector          */
/*                  scheduling                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SPI_Flash_Write(UINT32 destAddr, UINT8* data, UINT32 size);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_SectorErase                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  destAddr - address of the sector to erase                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine erase a sector from the flash                                             */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SPI_Flash_SectorErase(UINT32 destAddr);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_BulkErase                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  destAddr - address of the device                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine erase a sector from the flash                                             */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SPI_Flash_BulkErase(UINT32 destAddr);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_ReadStatus                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - device number to read from                                                    */
/*                  status - status variable to read into                                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads flash status register                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SPI_Flash_ReadPID(UINT32 devNum, UINT8 *pid0, UINT16 *pid1 );




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_ReadPID                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum  - device number to read from                                                   */
/*                  pid0    -                                                                              */
/*                  pid1    -                                                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads Flash device PID                                                    */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SPI_Flash_ReadStatus(UINT32 devNum, UINT8 *status);




#endif /*_FLAHS_SPI_H */

