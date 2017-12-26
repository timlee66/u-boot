/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   spi_flash_common.c                                                                                    */
/*            This file contains Common SPI flash low-level driver                                         */
/*  Project:                                                                                               */
/*            Yarkon HAL                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

#include "spi_flash_common.h"
#include "../../../Chips/chip_if.h"



/*---------------------------------------------------------------------------------------------------------*/
/*                                              API Functions                                              */
/*---------------------------------------------------------------------------------------------------------*/


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
void SPI_Flash_Common_GetStatus(UINT32 dev_num, UINT8* status)
{
	FIU_UMA_Read(dev_num, SPI_READ_STATUS_REG_CMD, 0, 0, status, 1);
}



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
void SPI_Flash_Common_GetID(UINT32 dev_num, UINT8 *pid0, UINT16 *pid1 )
{
	UINT32 data;
	UINT32 temp;

	FIU_UMA_Read(dev_num, SPI_READ_JEDEC_ID_CMD, 0x0, FALSE, (UINT8*)&data, 3);

	*pid0 = (data & 0x0000FF);
	temp = (data & 0xFF0000) >> 16;
	*pid1 = (data & 0x00FF00) | temp;
}


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
void SPI_Flash_Common_ExtendedAddrW(UINT32 dev_num, UINT8 HighAddr)
{
    FIU_UMA_Write(dev_num, SPI_WRITE_ENABLE_CMD, 0, FALSE, NULL, 0);
    FIU_UMA_Write(dev_num, SPI_WRITE_EXTENDED_ADDR_REG_CMD, 0, FALSE, &HighAddr, sizeof(UINT8));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_Erase                                                                 */
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
void SPI_Flash_Common_SectorErase(UINT32 dev_num, UINT32 addr)
{
     UINT32 high_addr = addr >> 24;
     UINT32 low_addr = addr & 0x00FFFFFF;

	 if (high_addr)
	 {
		SPI_Flash_Common_ExtendedAddrW(dev_num, high_addr);
	 }

     FIU_UMA_Write(
         dev_num,                           /* only one flash device */
         SPI_WRITE_ENABLE_CMD,              /* write enable transaction code */
         0,                                 /* address irrelevant */
         FALSE,                             /* no address for transaction */
         NULL,                              /* no write data */
         0);                                /* no data */

     FIU_UMA_Write(
         dev_num,                           /* only one flash device */
         SPI_4K_SECTOR_ERASE_CMD,           /* sector erase transaction code */
         low_addr,                          /* address relevant */
         TRUE,                              /* address for transaction */
         NULL,                              /* no write data */
         0);                                /* no data */

	SPI_Flash_Common_WaitTillReady(dev_num);

    if (high_addr)
    {
		SPI_Flash_Common_ExtendedAddrW(dev_num, 0);
    }
}




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
void SPI_Flash_Common_BulkErase(UINT32 dev_num)
{
     FIU_UMA_Write(
         dev_num,                           /* only one flash device */
         SPI_WRITE_ENABLE_CMD,              /* write enable transaction code */
         0,                                 /* address irrelevant */
         FALSE,                             /* no address for transaction */
         NULL,                              /* no write data */
         0);                                /* no data */

     FIU_UMA_Write(
         dev_num,                           /* only one flash device */
         SPI_CHIP_ERASE_CMD,                /* bulk erase transaction code */
         0,                                 /* no address is needed */
         0,                                 /*          " */
         NULL,                              /* no write data */
         0);                                /* no data */

	SPI_Flash_Common_WaitTillReady(dev_num);

}






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
void SPI_Flash_Common_Write(UINT32 devID, UINT32 destAddr, UINT8* data, UINT32 size)
{
    UINT32 high_addr = destAddr >> 24;
    UINT32 low_addr = destAddr & 0x00FFFFFF;

    if (high_addr)
    {
		SPI_Flash_Common_ExtendedAddrW(devID, high_addr);
    }

	/*-----------------------------------------------------------------------------------------------------*/
	/* Write Flash Using 256 Page EXTENDED MODE                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_UMA_Write(devID, SPI_WRITE_ENABLE_CMD, 0, FALSE, NULL, 0);

	FIU_ManualWrite(devID, SPI_PAGE_PRGM_CMD, low_addr, data, size);

	SPI_Flash_Common_WaitTillReady(devID);

    if (high_addr)
    {
		SPI_Flash_Common_ExtendedAddrW(devID, 0);
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Common_WaitTillReady                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine waits till the flash device is ready                                      */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_Common_WaitTillReady(UINT32 dev_num)
{
	UINT8 busy=1;

	do
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Reading flash status register                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        SPI_Flash_Common_GetStatus(dev_num, &busy);

        /*-------------------------------------------------------------------------------------------------*/
        /* Keep only "busy" bit 0                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        busy &= 0x01;

	} while (busy);
}


