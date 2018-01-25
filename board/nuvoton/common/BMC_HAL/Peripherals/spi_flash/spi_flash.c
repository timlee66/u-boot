/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   spi_flash.c                                                                                           */
/*            This file contains SPI flash logical driver                                                  */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#include "../../Common/hal_common.h"

#ifndef NO_LIBC
#include <string.h>
#else
int printf(const char* fmt, ...);
#endif

#include "spi_flash.h"
#include "common/spi_flash_common.h"

#include "../../Modules/fiu/fiu_if.h"
#include "../../Chips/chip_if.h"

/*#define VERBOSE_SPI_FLASH          // Debug */


#ifdef __LINUX_KERNEL_ONLY__
#define SPI_FLASH_SKIP_ERASE_ON_WRITE       /* Kernel MTD driver Erase before write anyway. */
#endif

/* Including SPI FLASH COMMON Driver Code  */
#include "common/spi_flash_common.c"


/*---------------------------------------------------------------------------------------------------------*/
/* Supported Flashes IDs                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

/*                                  PID0 |  PID1 */
/*                                 -------------- */
#define WINBOND_MF_ID               0xEF
	#define W25Q40_CAPACITY_ID              0x4013
	#define W25Q80_CAPACITY_ID              0x4014
	#define W25Q32_CAPACITY_ID              0x4016
	#define W25Q64_CAPACITY_ID              0x4017
	#define W25Q128_CAPACITY_ID             0x4018
	#define W25Q256_CAPACITY_ID             0x4019
	#define W25P80_CAPACITY_ID              0x2014
	#define W25P16_CAPACITY_ID              0x2015
	#define W25P32_CAPACITY_ID              0x2016

#define STMICRO_MF_IF               0x20
	#define M25P16_CAPACITY_ID              0x2015
	#define M25P32_CAPACITY_ID              0x2016
	#define M25P128_CAPACITY_ID             0x2018

#define SPANSION_MF_IF              0x01
	#define S25FS128_CAPACITY_ID              0x2018
	#define S25FS256_CAPACITY_ID              0x0219

#define MACRONIX_MF_IF              0xC2
	#define MX25L32_CAPACITY_ID            0x2016
	#define MX25L64_CAPACITY_ID            0x2017
	#define MX25L128_CAPACITY_ID           0x2018

#define DUMMY_MF_IF                 0xFF
	#define DUMMY_CAPACITY_ID              0xFFFF

/*---------------------------------------------------------------------------------------------------------*/
/* Supported flashes array                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static const flash_t supported_flash[]=
{
	{MACRONIX_MF_IF, MX25L32_CAPACITY_ID,  (UINT8*)"MX25L32",  _4MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25Q40_CAPACITY_ID,   (UINT8*)"W25Q40",   _512KB_, _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25Q80_CAPACITY_ID,   (UINT8*)"W25Q80",   _1MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25Q32_CAPACITY_ID,   (UINT8*)"W25Q32",   _4MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25Q64_CAPACITY_ID,   (UINT8*)"W25Q64",   _8MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25Q128_CAPACITY_ID,  (UINT8*)"W25Q128",  _16MB_,  _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25Q256_CAPACITY_ID,  (UINT8*)"W25Q256",  _32MB_,  _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25P80_CAPACITY_ID,   (UINT8*)"W25P80",   _1MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25P16_CAPACITY_ID,   (UINT8*)"W25P16",   _2MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{WINBOND_MF_ID,  W25P32_CAPACITY_ID,   (UINT8*)"W25P32",   _4MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{STMICRO_MF_IF,  M25P16_CAPACITY_ID,   (UINT8*)"M25P16",   _2MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{STMICRO_MF_IF,  M25P32_CAPACITY_ID,   (UINT8*)"M25P32",   _4MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{STMICRO_MF_IF,  M25P128_CAPACITY_ID,  (UINT8*)"M25P128",  _16MB_,  _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{SPANSION_MF_IF, S25FS128_CAPACITY_ID, (UINT8*)"S25FS128", _16MB_,  _4KB_,  _512B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{SPANSION_MF_IF, S25FS256_CAPACITY_ID, (UINT8*)"S25FS256", _32MB_,  _4KB_,  _512B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{MACRONIX_MF_IF, MX25L64_CAPACITY_ID,  (UINT8*)"MX25L64",  _8MB_,   _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{MACRONIX_MF_IF, MX25L128_CAPACITY_ID, (UINT8*)"MX25L128", _16MB_,  _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase},
	{0},
	{DUMMY_MF_IF, DUMMY_CAPACITY_ID, (UINT8*)"Unknown-Flash!!!",  _16MB_,  _4KB_,  _256B_, 0, SPI_Flash_Common_Write, SPI_Flash_Common_SectorErase, SPI_Flash_Common_GetStatus, SPI_Flash_Common_BulkErase}
};

/*---------------------------------------------------------------------------------------------------------*/
/* Defines BASED on the supported flashes array                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MAX_ERASE_BLOCK_SIZE            _256KB_
#define MAX_PROGRAM_PAGE_SIZE           _256B_

/*---------------------------------------------------------------------------------------------------------*/
/* Verbose prints                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(VERBOSE_GLOBAL) || defined(VERBOSE_SPI_FLASH)
	#ifndef __LINUX_KERNEL_ONLY__
	#define SPI_FLASH_MSG_DEBUG(fmt,args...)   printf(fmt ,##args)
	#else
	#define SPI_FLASH_MSG_DEBUG(fmt,args...)   printk(fmt ,##args)
	#endif
#else
	#define SPI_FLASH_MSG_DEBUG(fmt,args...)
#endif

#ifndef __LINUX_KERNEL_ONLY__
	#define SPI_FLASH_MSG(fmt,args...)   printf(fmt ,##args)
#else
	#define SPI_FLASH_MSG(fmt,args...)   printk(fmt ,##args)
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* API Variables definition                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
flash_t SPI_Flash_Devices[CONFIG_SYS_MAX_FLASH_BANKS] = {{0}};
UINT32  SPI_Flash_TotalSize = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Local Variables                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32 SPI_Flash_BaseAddress = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Local functions declarations                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
INT32   SPI_Flash_Addr2Sector_L         (UINT32 dev_num, UINT32 addr);
INT32   SPI_Flash_GetDeviceIDbyAddr_L   (UINT32 addr);
void    SPI_Flash_WritePageAligned_L    (UINT32 dev_num, UINT8 *src, UINT32 addr, UINT32 cnt);
INT32   SPI_Flash_GetDeviceIndexbyAddr_L(UINT32 addr);


/*---------------------------------------------------------------------------------------------------------*/
/*                                              API Functions                                              */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Init                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SPI flash initialization                                         */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SPI_Flash_Init(UINT32 flashBaseAddress)
{
	UINT8   pid0            = 0;
	UINT16  pid1            = 0;
	UINT32  fl_idx          = 0;
	UINT32  idx             = 0;
	UINT32  dev_cnt         = 0;
	UINT32  max_dev_size    = 0;

	SPI_Flash_BaseAddress   = flashBaseAddress;
	SPI_Flash_TotalSize = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* enable flashes 0-3                                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	/* Comment the following line since u-boot header determine which CS is enabled and if quad is enabled */
	/*GCR_MuxFIU(FIU_MODULE_0,TRUE,FALSE,FALSE,FALSE,TRUE); */


	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking all connected SPI devices                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	for(fl_idx=0; fl_idx < CONFIG_SYS_MAX_FLASH_BANKS; fl_idx++ )
	{
        SPI_Flash_ReadPID(fl_idx, &pid0, &pid1);

        SPI_FLASH_MSG_DEBUG("\nSPI_Flash: fl_idx=%d, pid0=0x%x, pid1=0x%x \n",fl_idx, pid0, pid1);

        /*-------------------------------------------------------------------------------------------------*/
        /* Checking all supported flash devices                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        idx = 0;
        while(supported_flash[idx].mf_id != 0)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Checking if supported flash device match current SPI flash ID                               */
            /*---------------------------------------------------------------------------------------------*/
            if((supported_flash[idx].mf_id == pid0) && (supported_flash[idx].capacity_id == pid1))
            {
                dev_cnt++;

                /*-----------------------------------------------------------------------------------------*/
                /* Flash device identified, moving to next one                                             */
                /*-----------------------------------------------------------------------------------------*/
                break;
            }

            idx++; /* continue to next in loop */
        }

        if ((supported_flash[idx].mf_id == 0) && (pid0 != 0))
        {
			idx++; /* increment to dummy device */
        }

		/* check if valid device */
		if ((pid1 & 0xFF) >=0x14 &&  (pid1 & 0xFF) <= 0x1B)
		{
              SPI_Flash_Devices[fl_idx]           = supported_flash[idx];

#if defined (NPCM750)
              SPI_Flash_Devices[fl_idx].startAddr = FLASH_BASE_ADDR(fl_idx);
			  if (SPI_Flash_Devices[fl_idx].mf_id == DUMMY_MF_IF)
			  {
			      SPI_Flash_Devices[fl_idx].chip_size = 2UL << (pid1-1);
			  }
#else
              SPI_Flash_Devices[fl_idx].startAddr = SPI_Flash_BaseAddress + SPI_Flash_TotalSize;
#endif
              SPI_Flash_TotalSize  += SPI_Flash_Devices[fl_idx].chip_size;

              if(max_dev_size < supported_flash[idx].chip_size)
              {
                  max_dev_size = supported_flash[idx].chip_size;
              }

			  SPI_FLASH_MSG("\nSPI_Flash%d: Found CS%d dev#%d Name[%s] ChipSize[0x%x]\n",
							 3*(fl_idx / FIU_DEVICES_PER_MODULE),
							 (fl_idx % FIU_DEVICES_PER_MODULE),
							 fl_idx,
							 SPI_Flash_Devices[fl_idx].name,
							 SPI_Flash_Devices[fl_idx].chip_size);

			  SPI_FLASH_MSG_DEBUG("\nSPI_Flash%d: Found CS%d dev#%d Name[%s] ChipSize[0x%x] SectorSize=[0x%x] PageSize[0x%x] StartAddr[0x%x]\n",
									3*(fl_idx / FIU_DEVICES_PER_MODULE),
									(fl_idx % FIU_DEVICES_PER_MODULE),
									fl_idx,
									SPI_Flash_Devices[fl_idx].name,
									SPI_Flash_Devices[fl_idx].chip_size,
									SPI_Flash_Devices[fl_idx].page_size,
									SPI_Flash_Devices[fl_idx].startAddr);
		}
		else
		{
		   /*-------------------------------------------------------------------------------------------------*/
		   /* Flash device wasn't identified!!                                                            */
		   /*-------------------------------------------------------------------------------------------------*/
			 SPI_FLASH_MSG_DEBUG("\nSPI_Flash: Flash Device #%d with ID[0x%X] wasn't identified!\n", fl_idx, pid0);
		}
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configuring the FIU with the total flash size                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	return FIU_Init(dev_cnt, max_dev_size, SPI_Flash_TotalSize);
}


#if !defined(SPI_FLASH_SKIP_ERASE_ON_WRITE)
void SPI_Flash_CheckAndDoEraseBoforeProgram_L(INT32 devNum, UINT32 destAddr, UINT8* data, UINT32 size)
{
	UINT32  start_s;
	UINT32  end_s;
	UINT32  sector_size;
	UINT32  page_size;
	UINT32  cur_sec;
	UINT32  char_count = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* First we calculate the sectors range involved with the writing                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	start_s     = SPI_Flash_Addr2Sector_L(devNum, destAddr);
	end_s       = SPI_Flash_Addr2Sector_L(devNum, destAddr + size - 1);

	sector_size    = SPI_Flash_Devices[devNum].sector_size;
	page_size      = SPI_Flash_Devices[devNum].page_size;

	SPI_FLASH_MSG_DEBUG("SPI_Flash: Start sector = %d, End sector = %d, Sector size = %d\n", start_s, end_s, sector_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* We check every sector if erase is needed                                                            */
	/*-----------------------------------------------------------------------------------------------------*/

	for (cur_sec = start_s; cur_sec <= end_s; ++cur_sec)
	{
        UINT32 check_addr;
        UINT32 sec_start_addr = SPI_Flash_Devices[devNum].startAddr + cur_sec * sector_size;

        UINT32 start_addr = sec_start_addr > destAddr ? sec_start_addr : destAddr;
        UINT32 end_addr   = sec_start_addr + sector_size - 1 < destAddr + size - 1 ? sec_start_addr + sector_size - 1 : destAddr + size - 1;

        BOOLEAN need_programming = FALSE;
        UINT8* tmp_src = data;

        if (char_count % 256)
        {
            if ((char_count % 4) == 0)
            {
                SPI_FLASH_MSG("#");
            }
        }
        else
        {
            SPI_FLASH_MSG("\n\t ");
        }
        char_count++;

        /*---------------------------------------------------------------------------------------------*/
        /* We check all the relevat data in the sector against the source.                             */
        /* if the data on the flash can be converted to source data without sector erase               */
        /* we skip directly to the writing procedure, otherwise we first erase the whole sector.       */
        /*---------------------------------------------------------------------------------------------*/
        for (check_addr = start_addr; check_addr <= end_addr; check_addr++, tmp_src++)
        {
            if ((*(UINT8*)check_addr) != (*(UINT8*)tmp_src))
            {
                need_programming = TRUE;
                if ( ((*(UINT8*)check_addr) & (*(UINT8*)tmp_src)) != (*(UINT8*)tmp_src))
                {
                    /*-------------------------------------------------------------------------------------*/
                    /*                                  Erase is needed!                                   */
                    /*-------------------------------------------------------------------------------------*/
                    UINT8   sector_buff[MAX_ERASE_BLOCK_SIZE];

                    SPI_FLASH_MSG_DEBUG("SPI_Flash: Flashing sector %5d, Start addr = 0x%X, End addr = 0x%X\n", cur_sec, start_addr, end_addr);

                    /*-------------------------------------------------------------------------------------*/
                    /* First we read the sector into buffer                                                */
                    /*-------------------------------------------------------------------------------------*/
                    memcpy(sector_buff, (UINT8*)(sec_start_addr), sector_size);

                    /*-------------------------------------------------------------------------------------*/
                    /* Second apply the new data to the buffer                                             */
                    /*-------------------------------------------------------------------------------------*/
                    memcpy((UINT8*)(sector_buff + (start_addr - sec_start_addr)), data,     \
                                        end_addr - start_addr + 1);

                    /*-------------------------------------------------------------------------------------*/
                    /* Third erase the sector                                                              */
                    /*-------------------------------------------------------------------------------------*/
                    SPI_Flash_SectorErase(sec_start_addr);

                    /*-------------------------------------------------------------------------------------*/
                    /* Forth write the new sector                                                          */
                    /*-------------------------------------------------------------------------------------*/
                    SPI_Flash_WritePageAligned_L(devNum, sector_buff, sec_start_addr, sector_size);

                    /*-------------------------------------------------------------------------------------*/
                    /* finished checking current sector                                                    */
                    /*-------------------------------------------------------------------------------------*/
                    need_programming = FALSE;
                    break;
                }
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* if the above check loop finished without erasing we only need to write the data                 */
        /*-------------------------------------------------------------------------------------------------*/
        if (need_programming)
        {
            tmp_src = data;
            for (check_addr = start_addr; check_addr <= end_addr;)
            {
                /* check if need programming */
                if ((*(UINT8*)check_addr) != (*(UINT8*)tmp_src))
                {
                    UINT32 current_page_size;
                    UINT32 current_page_end = check_addr | (page_size-1);

                    if (current_page_end > end_addr)
                        current_page_end = end_addr;
                    current_page_size = current_page_end - check_addr + 1;

                    SPI_FLASH_MSG_DEBUG("SPI_Flash: Flashing page Start addr = 0x%X, End addr = 0x%X\n", check_addr, current_page_end);

                    /* program to end of current page or end of data */
                    SPI_Flash_WritePageAligned_L(devNum, tmp_src, check_addr, current_page_size);
                    check_addr += current_page_size;
                    tmp_src += current_page_size;
                }
                else
                {
                    check_addr++;
                    tmp_src++;
                }
            }
        }

        data += end_addr - start_addr + 1;
	}

	if ((char_count % 256) != 1)
	{
        SPI_FLASH_MSG("\n\t ");
	}
}
#endif



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
HAL_STATUS SPI_Flash_Write(UINT32 destAddr, UINT8* data, UINT32 size)
{
	INT32   devNum;

	SPI_FLASH_MSG_DEBUG("\n");

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculating the device ID from the address                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	devNum = SPI_Flash_GetDeviceIndexbyAddr_L(destAddr);

	SPI_FLASH_MSG_DEBUG("SPI_Flash: Device Num= %d \n", devNum);

	if (devNum == -1)
        return HAL_ERROR_BAD_DEVNUM;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking if the driver was initialized                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (!(SPI_Flash_Devices[devNum].FlashPageProg))
	{
        SPI_FLASH_MSG_DEBUG("SPI_Flash: Device Number %d wasn't initialized\n", devNum);
        return HAL_ERROR_NOT_INITIALIZED;
	}


	SPI_FLASH_MSG_DEBUG("SPI_Flash: Destination addr = 0x%X, Source addr = 0x%X, size = %d\n", destAddr, data, size);

#if defined(SPI_FLASH_SKIP_ERASE_ON_WRITE)

	SPI_Flash_WritePageAligned_L(devNum, data, destAddr, size);

#else /* !defined(SPI_FLASH_SKIP_ERASE_ON_WRITE) */
	SPI_Flash_CheckAndDoEraseBoforeProgram_L(devNum, destAddr, data, size);
#endif

	return HAL_OK;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Erase                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  destAddr - address of the sector to erase                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine erase a sector from the flash                                             */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS SPI_Flash_SectorErase(UINT32 destAddr)
{
	UINT32 localAddr;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculating the device ID from the address                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	INT32 devNum = SPI_Flash_GetDeviceIndexbyAddr_L(destAddr);

	SPI_FLASH_MSG_DEBUG("SPI_Flash: Device Num= %d Erasing address 0x%X\n", devNum, destAddr);

	if (devNum == -1)
        return HAL_ERROR_BAD_DEVNUM;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking if the driver was initialized                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (!(SPI_Flash_Devices[devNum].FlashSectorErase))
	{
        SPI_FLASH_MSG_DEBUG("SPI_Flash: Device Number %d wasn't initialized\n", devNum);
        return HAL_ERROR_NOT_INITIALIZED;
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculating local address on the flash                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	localAddr = destAddr - SPI_Flash_Devices[devNum].startAddr;

	if (localAddr % SPI_Flash_Devices[devNum].sector_size != 0)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Error: address for erase must point to beginning of Erase Sector                                */
        /*-------------------------------------------------------------------------------------------------*/
        SPI_FLASH_MSG_DEBUG("SPI_Flash: address 0x%X for erase must point to beginning of Erase Sector\n", localAddr);

        return HAL_ERROR_BAD_ADDRESS;
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Finally executing the drivers erase function                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SPI_FLASH_MSG_DEBUG("SPI_Flash: Erasing sector from address 0x%X\n", destAddr);
	SPI_Flash_Devices[devNum].FlashSectorErase(devNum, localAddr);

	return HAL_OK;
}


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
HAL_STATUS SPI_Flash_BulkErase(UINT32 destAddr)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculating the device ID from the address                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	INT32 devNum = SPI_Flash_GetDeviceIndexbyAddr_L(destAddr);

	if (devNum == -1)
        return HAL_ERROR_BAD_DEVNUM;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking if the driver was initialized                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (!(SPI_Flash_Devices[devNum].FlashBulkErase))
	{
        SPI_FLASH_MSG_DEBUG("SPI_Flash: Device Number %d wasn't initialized\n", devNum);
        return HAL_ERROR_NOT_INITIALIZED;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Finally executing the drivers erase function                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SPI_FLASH_MSG_DEBUG("SPI_Flash: Erasing flash device number %d\n", devNum);
	SPI_Flash_Devices[devNum].FlashBulkErase(devNum);

	return HAL_OK;
}


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
HAL_STATUS SPI_Flash_ReadStatus(UINT32 devNum, UINT8 *status)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= CONFIG_SYS_MAX_FLASH_BANKS)
	{
        SPI_FLASH_MSG_DEBUG("SPI_Flash: Error Device Number %d\n", devNum);
        return HAL_ERROR_BAD_DEVNUM;
	}

	if (!(SPI_Flash_Devices[devNum].FlashReadStatus))
	{
        SPI_FLASH_MSG_DEBUG("SPI_Flash: Device Number %d wasn't initialized\n", devNum);
        return HAL_ERROR_NOT_INITIALIZED;
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Redirecting to actual driver                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SPI_Flash_Devices[devNum].FlashReadStatus(devNum, status);

	return HAL_OK;
}



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
HAL_STATUS SPI_Flash_ReadPID(UINT32 devNum, UINT8 *pid0, UINT16 *pid1 )
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= CONFIG_SYS_MAX_FLASH_BANKS)
	{
        SPI_FLASH_MSG_DEBUG("SPI_Flash: Error Device Number %d\n", devNum);
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Redirecting to COMMON driver, as the actual driver can't be initialize yet                          */
	/* (have to know the device ID)                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SPI_Flash_Common_GetID(devNum, pid0, pid1);

	return HAL_OK;
}







/*---------------------------------------------------------------------------------------------------------*/
/*                                             Local Functions                                             */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_GetDeviceIndexbyAddr_L                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  addr - memory address to check                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine gets flash device index out of its memory mapped address                  */
/*---------------------------------------------------------------------------------------------------------*/
INT32 SPI_Flash_GetDeviceIndexbyAddr_L(UINT32 addr)
{
	INT32 idx     = 0;

	for (idx=0; idx<CONFIG_SYS_MAX_FLASH_BANKS; ++idx)
	{
        if ( (SPI_Flash_Devices[idx].startAddr <= addr) &&
             (SPI_Flash_Devices[idx].startAddr + SPI_Flash_Devices[idx].chip_size > addr) )
        {
            /*---------------------------------------------------------------------------------------------*/
            /* We found the dev ID                                                                         */
            /*---------------------------------------------------------------------------------------------*/
            return idx;
        }

	}
	SPI_FLASH_MSG_DEBUG("SPI_Flash: Device Number %d wasn't found\n", idx);
	return -1;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_Addr2Sector_L                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num - Device index                                                                 */
/*                  addr    - Memory mapped address                                                        */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine converts given address mapped to flash and returns the erase sector index */
/*                  related to the given memory address                                                    */
/*---------------------------------------------------------------------------------------------------------*/
INT32 SPI_Flash_Addr2Sector_L(UINT32 dev_num, UINT32 addr)
{
	return (addr - SPI_Flash_Devices[dev_num].startAddr) / SPI_Flash_Devices[dev_num].sector_size;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SPI_Flash_WritePageAligned_L                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dev_num - Device index                                                                 */
/*                  src     - Source data address                                                          */
/*                  addr    - Destination address                                                          */
/*                  cnt     - Data size                                                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs flash page programming by pages chunks                           */
/*---------------------------------------------------------------------------------------------------------*/
void SPI_Flash_WritePageAligned_L(UINT32 dev_num, UINT8 *src, UINT32 addr, UINT32 cnt)
{
	UINT32  local_addr      = addr - SPI_Flash_Devices[dev_num].startAddr;
	UINT32  pageSize        = SPI_Flash_Devices[dev_num].page_size;
	UINT32  actual_size     = 0;

	if(cnt != 0)
	{
        while (cnt)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Calculating the size from current address to the start of the next page                     */
            /*---------------------------------------------------------------------------------------------*/
            actual_size = ((((local_addr)/pageSize) + 1)*pageSize) - (local_addr);

            /*---------------------------------------------------------------------------------------------*/
            /* If we smaller amount to write                                                               */
            /*---------------------------------------------------------------------------------------------*/
            if (actual_size > cnt)
                actual_size = cnt;

            /*---------------------------------------------------------------------------------------------*/
            /* Executing the Page Programming                                                              */
            /*---------------------------------------------------------------------------------------------*/
            SPI_Flash_Devices[dev_num].FlashPageProg(dev_num, local_addr, src, actual_size);

            /*---------------------------------------------------------------------------------------------*/
            /* Updating loop variables                                                                     */
            /*---------------------------------------------------------------------------------------------*/
            src         += actual_size;
            local_addr  += actual_size;
            cnt         -= actual_size;
        }
	}
}



