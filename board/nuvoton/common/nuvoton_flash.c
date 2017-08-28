/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nuvoton_flash.c                                                                                       */
/*            This file contains flash api for U-BOOT                                                      */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

#include "common.h"
#include "config.h"
#include "spi.h"
#include "flash.h"
#include "linux/sizes.h"

#undef MAX
#undef MIN

#include "BMC_HAL/Boards/board.h"

#ifndef CONFIG_SYS_NO_FLASH
/*---------------------------------------------------------------------------------------------------------*/
/* Flash info array                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        flash_init                                                                             */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs flash initialization                                             */
/*---------------------------------------------------------------------------------------------------------*/
unsigned long flash_init (void)
{
	INT32 idx         = 0;
	INT32 sect_idx    = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initializing the SPI Flash                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	SPI_Flash_Init(CONFIG_FLASH_BASE);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Constructing the flash_info array                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/

	/* Init: no FLASHes known */
	for (idx = 0; idx < CONFIG_SYS_MAX_FLASH_BANKS; ++idx) {
		flash_info[idx].flash_id = FLASH_UNKNOWN;
	}

	for (idx=0; idx < CONFIG_SYS_MAX_FLASH_BANKS; ++idx)
	{
        if (SPI_Flash_Devices[idx].mf_id != 0)
        {
            flash_info[idx].size            = SPI_Flash_Devices[idx].chip_size;
            flash_info[idx].sector_count    = SPI_Flash_Devices[idx].chip_size/SPI_Flash_Devices[idx].sector_size;
            flash_info[idx].flash_id        = SPI_Flash_Devices[idx].capacity_id << 16 | SPI_Flash_Devices[idx].mf_id;

            for(sect_idx=0; sect_idx < flash_info[idx].sector_count; sect_idx++)
            {
                flash_info[idx].start[sect_idx] = SPI_Flash_Devices[idx].startAddr +              \
                                                  SPI_Flash_Devices[idx].sector_size * sect_idx;
                flash_info[idx].protect[sect_idx] = 0;
            }
        }
	}

	return SPI_Flash_TotalSize;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        flash_print_info                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  flash_info_ptr - pointer to the flash into entry                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine  prints info about the given flash device                                 */
/*---------------------------------------------------------------------------------------------------------*/
void flash_print_info (flash_info_t *flash_info_ptr)
{
	INT32 sect_idx;

	printf(" size : [0x%lx]",    flash_info_ptr->size);
	printf(" sect count : [%d]", flash_info_ptr->sector_count);
	printf(" id : [0x%lx]\n",    flash_info_ptr->flash_id);

	for(sect_idx=0; sect_idx < flash_info_ptr->sector_count; sect_idx++)
	{
        printf("[0x%lx][%s]",
                flash_info_ptr->start[sect_idx],
                (flash_info_ptr->protect[sect_idx])?"(RO)":"(RW)");

        if(sect_idx + 1 % 6 == 0)
        {
            printf("\n");
        }
	}

	printf("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        flash_erase                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  flash_info_ptr  - pointer to flash device info entry                                   */
/*                  s_first         - first erase sector index                                             */
/*                  s_last          - last erase sector index                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs flash erase inside given flash device                            */
/*---------------------------------------------------------------------------------------------------------*/
int flash_erase (flash_info_t * flash_info_ptr, int s_first, int s_last)
{
	HAL_STATUS ret = HAL_OK;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking if we need erase whole flash device                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	if ((s_first == 0) && ((s_last + 1) == flash_info_ptr->sector_count))
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* If we do, we execute optimized erase                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        ret = SPI_Flash_BulkErase(flash_info_ptr->start[s_first]);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Otherwise we erase sectors in the range                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	else
	{
        UINT32 sect_idx = 0;
        for(sect_idx = s_first; sect_idx <= s_last; ++sect_idx)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Erase it using SPI Flash driver                                                             */
            /*---------------------------------------------------------------------------------------------*/
            ret += SPI_Flash_SectorErase(flash_info_ptr->start[sect_idx]);
        }
	}

	if (ret == HAL_OK)
        return 0;
	else
        return -ret;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        write_buff                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  flash_info_ptr  - flash device info                                                    */
/*                  addr            - destination address on a flash                                       */
/*                  src             - source address                                                       */
/*                  cnt             - source data size                                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes a buffer to the flash                                              */
/*---------------------------------------------------------------------------------------------------------*/
int write_buff (flash_info_t *flash_info_ptr, uchar *src, ulong addr, ulong cnt)
{
	INT ret;

	if ((ret = SPI_Flash_Write((UINT32)addr, src, cnt)) == HAL_OK)
        return 0;
	else
        return -ret;
}
#endif


