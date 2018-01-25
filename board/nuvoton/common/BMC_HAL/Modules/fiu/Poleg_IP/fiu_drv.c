/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   fui_drv.c                                                                                             */
/*            This file contains interface to Flash Interface Unit (FUI)                                   */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#include "../../../Chips/chip_if.h"

#include "fiu_regs.h"
#include "fiu_drv.h"
#include "fiu_defs.h"

#define WRITE_TRANSFER_16_BYTES

#ifdef WRITE_TRANSFER_16_BYTES      /* command=1  data=15 */
#define CHUNK_SIZE  16
#endif

#ifdef WRITE_TRANSFER_17_BYTES      /* command=1 data=16 */
#define CHUNK_SIZE  17
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Global define:  FIU_module holds the FIU current module number. It is selected using FIU_Select.        */
/*                 This is written like this as to not to change the IF for chips with a single FIU module.*/
/*---------------------------------------------------------------------------------------------------------*/
void *memcpy(void *dest, const void *src, UINT32 n);

/*---------------------------------------------------------------------------------------------------------*/
/* Local functions declarations                                                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigReadMode                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  num_device       - number of flash devices connected to the chip                       */
/*                  read_mode - FIU read mode                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the FIU read mode.                                             */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_ConfigReadMode (UINT num_device,
                         SPI_read_mode_t read_mode)
{
	FIU_MODULE_T FIU_module = (num_device < FIU_DEVICES_PER_MODULE) ? FIU_MODULE_0 : FIU_MODULE_3;

	SET_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_EXEC_DONE, 1);


	/*-----------------------------------------------------------------------------------------------------*/
	/* Change the access type. Warning: Once you go QUAD, you can't go back                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( read_mode == FIU_FAST_READ)
	{
        SET_REG_FIELD( FIU_DRD_CFG(FIU_module),  FIU_DRD_CFG_ACCTYPE, 0);
	}
	else if ( (read_mode == FIU_FAST_READ_DUAL_IO)  ||  (read_mode == FIU_FAST_READ_DUAL_OUTPUT) )
	{
        SET_REG_FIELD( FIU_DRD_CFG(FIU_module),  FIU_DRD_CFG_ACCTYPE, 1);
	}
#ifdef FIU_CAPABILITY_QUAD_READ
	else if (read_mode == FIU_FAST_READ_QUAD_IO)
	{
        SET_REG_FIELD( FIU_DRD_CFG(FIU_module),  FIU_DRD_CFG_ACCTYPE, 2);
	}
#endif
#ifdef FIU_CAPABILITY_SPI_X
	else if (read_mode == FIU_FAST_READ_SPI_X)
	{
        SET_REG_FIELD( FIU_DRD_CFG(FIU_module),  FIU_DRD_CFG_ACCTYPE, 3);
	}
#endif
	else
	{
        ASSERT(FALSE);
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigFlashSize                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  num_device       - number of flash devices connected to the chip                       */
/*                  flash_size - Flash size to configure                                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures FIU flash size                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_ConfigFlashSize(UINT num_device,
                         SPI_dev_size_t flash_size)
{
	FIU_MODULE_T FIU_module = (num_device < FIU_DEVICES_PER_MODULE) ? FIU_MODULE_0 : FIU_MODULE_3;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure flash address                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( flash_size <= FIU_DEV_SIZE_16MB )
	{
        SET_REG_FIELD( FIU_DRD_CFG(FIU_module) ,  FIU_DRD_CFG_ADDSIZ,  FIU_DRD_CFG_ADDSIZE_24BIT) ;
        SET_REG_FIELD( FIU_DWR_CFG(FIU_module) ,  FIU_DWR_CFG_ADDSIZ,  FIU_DRD_CFG_ADDSIZE_24BIT) ;
	}
	else
	{
        SET_REG_FIELD( FIU_DRD_CFG(FIU_module) ,  FIU_DRD_CFG_ADDSIZ,  FIU_DRD_CFG_ADDSIZE_32BIT) ;
        SET_REG_FIELD( FIU_DWR_CFG(FIU_module) ,  FIU_DWR_CFG_ADDSIZ,  FIU_DRD_CFG_ADDSIZE_32BIT) ;
	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ConfigBurstSize                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  num_device       - number of flash devices connected to the chip                       */
/*                  read_burst_size     - FIU read burst size                                              */
/*                  write_burst_size    - FIU write burst size                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures read/write burst size                                          */
/*---------------------------------------------------------------------------------------------------------*/
void FIU_ConfigBurstSize (UINT num_device,
                          SPI_r_burst_t read_burst_size,
                          SPI_w_burst_t write_burst_size)
{
	FIU_MODULE_T FIU_module = (num_device < FIU_DEVICES_PER_MODULE) ? FIU_MODULE_0 : FIU_MODULE_3;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure write burst size                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD( FIU_DWR_CFG(FIU_module),  FIU_DWR_CFG_W_BURST, write_burst_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure read burst size                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD( FIU_DRD_CFG(FIU_module),  FIU_DRD_CFG_R_BURST, read_burst_size);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_Init                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  num_device       - number of flash devices connected to the chip                       */
/*                  max_dev_size     - Size of the biggest device connected in bytes                       */
/*                  total_flash_size - The total flash size in bytes                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configs the FUI                                                           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FIU_Init(UINT num_device, UINT32 max_dev_size, UINT32 total_flash_size)
{
/* TODO: currently we don't change FIU modes */
#if 0
	SPI_dev_size_t              flash_size          = FIU_DEV_SIZE_128KB;
	SPI_read_mode_t             read_mode           = FIU_FAST_READ;
	SPI_w_burst_t               write_burst_size    = FIU_W_BURST_ONE_BYTE;
	SPI_r_burst_t               read_burst_size     = FIU_R_BURST_ONE_BYTE;

	/*-----------------------------------------------------------------------------------------------------*/
	/* config total flash size - if not 1(MB) align, align to 1(MB)                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	if (total_flash_size & (_512KB_ - 1))
	{
        total_flash_size = ROUND_UP(total_flash_size, _512KB_);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* convert from bytes to enum                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	flash_size = total_flash_size / _1MB_;


	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure Flash Size                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_ConfigFlashSize(num_device, flash_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure read mode                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_ConfigReadMode(num_device, read_mode);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure burst sizes                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_ConfigBurstSize(num_device, read_burst_size, write_burst_size);

#endif
	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_UMA_Read                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                device           - Select the flash device (0 or 1) to be accessed                       */
/*                transaction_code - Specify the SPI UMA transaction code                                  */
/*                address          - Location on the flash , in the flash address space                    */
/*                address_size     - if TRUE, 3 bytes address, to be placed in FIU_UMA_AB0-2                   */
/*                                   else (FALSE), no address for this SPI UMA transaction                 */
/*                data             - a pointer to a data buffer to hold the read data.                     */
/*                data_size        - buffer size. Legal sizes are 1,2,3,4                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Read up to 4 bytes from the flash. using the FIU User Mode Access (UMA)   */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FIU_UMA_Read(UINT     device,
                        UINT8    transaction_code,
                        UINT32   address,
                        BOOLEAN  is_address_size,
                        UINT8 *  data,
                        UINT32   data_size)
{

	UINT32 data_reg[4];
	UINT32 uma_cfg = 0x0;
	HAL_STATUS ret = HAL_OK;
	UINT32 address_size = 0;

	FIU_MODULE_T FIU_module = (device < FIU_DEVICES_PER_MODULE) ? FIU_MODULE_0 : FIU_MODULE_3;

	/*-----------------------------------------------------------------------------------------------------*/
	/* set device number - DEV_NUM in FIU_UMA_CTS                                                          */
	/* legal device numbers are 0,1,2,3                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	device = device % FIU_DEVICES_PER_MODULE;
	switch(device)
	{
        case 0 :
        case 1 :
            SET_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_DEV_NUM, (UINT32)device);
            break;
        default:
            return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* set transaction code in FIU_UMA_CODE                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CMD(FIU_module), FIU_UMA_CMD_CMD, transaction_code);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_CMDSIZ, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set address size bit                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (is_address_size)
	    address_size = 3;

	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_ADDSIZ, address_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA address registers                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_ADDR(FIU_module), address);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set data size and direction                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_RDATSIZ, data_size);
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_WDATSIZ, 0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set UMA CFG                                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_CFG(FIU_module), uma_cfg);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initiate the read                                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_EXEC_DONE, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait for indication that transaction has terminated                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	while (READ_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_EXEC_DONE) == FIU_TRANS_STATUS_IN_PROG){}

	/*-----------------------------------------------------------------------------------------------------*/
	/* copy read data from FIU_UMA_DB0-3 regs to data buffer                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA data registers - FIU_UMA_DB0-3                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	if (data_size >= FIU_UMA_DATA_SIZE_1)
	{
		data_reg[0] = REG_READ(FIU_UMA_DR0(FIU_module));
	}
	if (data_size >= FIU_UMA_DATA_SIZE_5)
	{
		data_reg[1] = REG_READ(FIU_UMA_DR1(FIU_module));
	}
	if (data_size >= FIU_UMA_DATA_SIZE_9)
	{
		data_reg[2] = REG_READ(FIU_UMA_DR2(FIU_module));
	}
	if (data_size >= FIU_UMA_DATA_SIZE_13)
	{
		data_reg[3] = REG_READ(FIU_UMA_DR3(FIU_module));
	}

	memcpy(data,data_reg,data_size);

	return ret;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_UMA_Write                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*              device           - Select the flash device (0 or 1) to be accessed                         */
/*              transaction_code - Specify the SPI UMA transaction code                                    */
/*              address          - Location on the flash, in the flash address space                       */
/*              address_size     - if TRUE, 3 bytes address, to be placed in FIU_UMA_AB0-2                     */
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
HAL_STATUS FIU_UMA_Write(
           UINT device,
           UINT8 transaction_code,
           UINT32 address,
           BOOLEAN is_address_size,
           UINT8 * data,
           UINT32 data_size)
{

	UINT32 data_reg[4] = {0};
	UINT32 uma_reg   = 0x0;
	HAL_STATUS ret = HAL_OK;
	UINT32 address_size = 0;

	FIU_MODULE_T FIU_module = (device < FIU_DEVICES_PER_MODULE) ? FIU_MODULE_0 : FIU_MODULE_3;

	/*-----------------------------------------------------------------------------------------------------*/
	/* set device number - DEV_NUM in FIU_UMA_CTS                                                          */
	/* legal device numbers are 0,1,2,3                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	device = device % FIU_DEVICES_PER_MODULE;
	switch(device)
	{
        case 0 :
        case 1 :
            SET_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_DEV_NUM, (UINT32)device);
            break;
        default:
            return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set transaction code (command byte source)                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CMD(FIU_module), FIU_UMA_CMD_CMD, transaction_code);
	SET_VAR_FIELD(uma_reg, FIU_UMA_CFG_CMDSIZ, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set address size bit                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (is_address_size)
	    address_size = 3;

	SET_VAR_FIELD(uma_reg, FIU_UMA_CFG_ADDSIZ, address_size);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA address registers                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_ADDR(FIU_module), address);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the UMA data registers - FIU_UMA_DB0-3                                                          */
	/*-----------------------------------------------------------------------------------------------------*/

	memcpy(data_reg,data,data_size);

	if (data_size >= FIU_UMA_DATA_SIZE_1)
	{
        REG_WRITE(FIU_UMA_DW0(FIU_module), data_reg[0]);
	}
	if (data_size >= FIU_UMA_DATA_SIZE_5)
	{
        REG_WRITE(FIU_UMA_DW1(FIU_module), data_reg[1]);
	}
	if (data_size >= FIU_UMA_DATA_SIZE_9)
	{
        REG_WRITE(FIU_UMA_DW2(FIU_module), data_reg[2]);
	}
	if (data_size >= FIU_UMA_DATA_SIZE_13)
	{
        REG_WRITE(FIU_UMA_DW3(FIU_module), data_reg[3]);
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Set data size and direction                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(uma_reg, FIU_UMA_CFG_WDATSIZ, data_size);
	SET_VAR_FIELD(uma_reg, FIU_UMA_CFG_RDATSIZ, 0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set UMA status                                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FIU_UMA_CFG(FIU_module), uma_reg);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initiate the read                                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_EXEC_DONE, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait for indication that transaction has terminated                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	while (READ_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_EXEC_DONE) == FIU_TRANS_STATUS_IN_PROG){}

	return ret;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FIU_ManualWrite                                                                        */
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
/*                  The data can be programmed upto the size of the whole page in a single SPI transaction */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FIU_ManualWrite(UINT device, UINT8 transaction_code, UINT32 address, UINT8 * data, UINT32 data_size)
{
	UINT8   uma_cfg  = 0x0;
	UINT32  num_data_chunks;
	UINT32  remain_data;
	UINT32  idx = 0;

	FIU_MODULE_T FIU_module = (device < FIU_DEVICES_PER_MODULE) ? FIU_MODULE_0 : FIU_MODULE_3;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Constructing var for FIU_UMA_CFG register status                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(uma_cfg, FIU_UMA_CFG_WDATSIZ, 16);        /* Setting Write Data size */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculating relevant data                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	num_data_chunks  = data_size / CHUNK_SIZE;
	remain_data  = data_size % CHUNK_SIZE;

	/*-----------------------------------------------------------------------------------------------------*/
	/* First we activate Chip Select (CS) for the given flash device                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_DEV_NUM, (UINT32)(device % FIU_DEVICES_PER_MODULE));
	SET_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_SW_CS, 0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Writing the transaction code and the address to the bus                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	FIU_UMA_Write(device, transaction_code, address, TRUE, NULL, 0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Starting the data writing loop in multiples of 8                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	for(idx=0; idx<num_data_chunks; ++idx)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* first byte command and follow 3 bytes address used as DATA                                                    */
        /*-------------------------------------------------------------------------------------------------*/
		FIU_UMA_Write(device, data[0], (UINT32)NULL, FALSE, &data[1], CHUNK_SIZE-1);

        data += CHUNK_SIZE;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Handling chunk remains                                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (remain_data > 0)
	{
		FIU_UMA_Write(device, data[0], (UINT32)NULL, FALSE, &data[1], remain_data-1);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Finally we de-activating the Chip select and returning to "automatic" CS control                    */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(FIU_UMA_CTS(FIU_module), FIU_UMA_CTS_SW_CS, 1);

	return HAL_OK;
}



