/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   spi_flash_common.h                                                                                    */
/*            This file contains Common SPI flash low-level driver interface                               */
/*  Project:                                                                                               */
/*            Yarkon HAL                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _FLASH_SPI_COMMON_H
#define _FLASH_SPI_COMMON_H


#include "defs.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Common SPI flash commands                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define SPI_READ_JEDEC_ID_CMD       0x9F
#define SPI_WRITE_ENABLE_CMD        0x06
#define SPI_WRITE_DISABLE_CMD       0x04
#define SPI_READ_STATUS_REG_CMD     0x05
#define SPI_WRITE_STATUS_REG_CMD    0x01
#define SPI_READ_DATA_CMD           0x03
#define SPI_PAGE_PRGM_CMD           0x02
#define SPI_4K_SECTOR_ERASE_CMD     0x20
#define SPI_32K_BLOCK_ERASE_CMD     0x52
#define SPI_64K_BLOCK_ERASE_CMD     0xD8
#define SPI_CHIP_ERASE_CMD          0xC7
#define SPI_WRITE_EXTENDED_ADDR_REG_CMD 0xC5


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_GetStatus                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num - device index                                                                 */
/*                  status  -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads Flash status register                                               */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_Common_GetStatus(UINT32 dev_num, UINT8* status);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_GetID                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num - device index                                                                 */
/*                  pid0 -                                                                                 */
/*                  pid1 -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads JEDEC standard Flash ID                                             */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_Common_GetID(UINT32 dev_num, UINT8 *pid0, UINT16 *pid1 );



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_SectorErase                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num -                                                                              */
/*                  addr    -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs sector erase                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_Common_SectorErase(UINT32 dev_num, UINT32 addr);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_BulkErase                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num -                                                                              */
/*                  addr    -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs bulk erase of all the flash device                               */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_Common_BulkErase(UINT32 dev_num);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_Write                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devID       - Flash device index                                                       */
/*                  destAddr    - Local destination address                                                */
/*                  data        - data ptr                                                                 */
/*                  size        - data size                                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs flash write to the given (by index) device                       */
/*                  The dest address can be NOT aligned to a page size, but the data must fit the current  */
/*                  page, if the data exceeds the page it will be overlaped and written from the beginning */
/*                  of the page (usual flash devices behavior)                                             */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_Common_Write(UINT32 devID, UINT32 destAddr, UINT8* data, UINT32 size);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_ExtendedAddrW                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num     - Flash device index                                                       */
/*                  HighAddr    - 4th byte address (bits 24-31)                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets the extended Address to HighAddr so commands with 3 so operations    */
/*                  with 3 bytes address will be able to access a higher area                              */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_Common_ExtendedAddrW(UINT32 dev_num, UINT8 HighAddr);



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_WaitTillReady_L                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine waits till the flash device is ready                                      */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_Common_WaitTillReady(UINT32 dev_num);

#endif /*_FLASH_SPI_COMMON_H */

