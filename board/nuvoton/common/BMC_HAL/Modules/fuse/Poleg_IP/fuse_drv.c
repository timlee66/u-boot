/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   fuse_drv.c                                                                                            */
/*            This file contains FUSE module routines                                                      */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#include "../../../Common/hal_common.h"
#include "../../../Chips/chip.h"

#include "../../aes/aes_if.h"
#include "../../clk/Poleg_IP/clk_regs.h"

#include "fuse_drv.h"
#include "fuse_regs.h"

/*#define CONFIG_FUSE_EMULATE */
#ifdef CONFIG_FUSE_EMULATE
	UINT8 *fuse_array_mem = (UINT8 *)0x2000000;
	#define FUSE_ARR_SIZE 1024
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Fuse module constant definitions                                                                        */
/*---------------------------------------------------------------------------------------------------------*/


/* Read cycle initiation value: */
#define READ_INIT                   0x02

/* Program cycle initiation values (a sequence of two adjacent writes is required): */
#define PROGRAM_ARM                 0x1
#define PROGRAM_INIT                0xBF79E5D0

/* Value to clean FDATA contents: */
#define FDATA_CLEAN_VALUE           0x01

/* Default APB Clock Rate (in MHz): */
#define DEFAULT_APB_RATE            0x30

#define MIN_PROGRAM_PULSES          4
#define MAX_PROGRAM_PULSES          20


/*---------------------------------------------------------------------------------------------------------*/
/* Fuse module local macro definitions                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
/* #define STORAGE_ARRAY_READY(sa)   READ_REG_FIELD(FST(sa),     FST_RDY) */
#define KEY_IS_VALID()            READ_REG_FIELD(FKEYIND,     FKEYIND_KVAL)
#define DISABLE_KEY_ACCESS()      SET_REG_FIELD(FCFG(KEY_SA), FCFG_FDIS, 1)      /* Lock OTP module access  */

static BOOLEAN FUSE_IsInitialized = FALSE;


/*---------------------------------------------------------------------------------------------------------*/
/* Internal functions for this module                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_WaitForOTPReadyWithTimeout                                                                              */
/*                                                                                                         */
/* Parameters:      array - fuse array to wait for                                                       */
/* Returns:         HAL_STATUS                                                                                   */
/* Side effects:                                                                                           */
/* Description:     Initialize the Fuse HW module.                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static HAL_STATUS FUSE_WaitForOTPReadyWithTimeout(FUSE_STORAGE_ARRAY_T array, UINT32 timeout)
{
	volatile UINT32 time = timeout;

	/*-----------------------------------------------------------------------------------------------------*/
	/* check parameters validity                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	if (array > FUSE_SA)
	{
        return HAL_ERROR_BAD_PARAM;
	}

	while (--time > 1)
	{
        if (READ_REG_FIELD(FST(array), FST_RDY))
        {
            /* fuse is ready, clear the status. */
            SET_REG_FIELD(FST(array), FST_RDST, 1);

            return HAL_OK;
        }
	}
	/* try to clear the status in case it was set */
	SET_REG_FIELD(FST(array), FST_RDST, 1);

	return HAL_ERROR;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_Init                                                                              */
/*                                                                                                         */
/* Parameters:      APBclock - APB clock rate in MHz                                                       */
/* Returns:         HAL_STATUS                                                                                   */
/* Side effects:                                                                                           */
/* Description:     Initialize the Fuse HW module.                                                         */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_Init (void)
{

	/* APBRT (APB Clock Rate). Informs the fuse array state machine on the APB clock rate in MHz. The */
	/* software must update this field before writing the OTP, and before APB4 actual clock rate change. The */
	/* state machine contains an internal copy of this field, sampled at the beginning of every read or program */
	/* operation. Software should not write this field with 0. The reset value of this field is 1Fh (31 MHz). The */
	/* accuracy of the setting should be 10%. */
	/* Note: The minimum APB allowed frequency for accessing the fuse arrays is 10 MHz. */

	UINT8 APBclock = CLK_GetAPBFreq(APB4)/1000000 + 1;

	/* Configure the Key Storage Array APB Clock Rate */
	SET_REG_FIELD(FCFG(KEY_SA), FCFG_APBRT, APBclock & 0x3F);

	/* Configure the Fuse Storage Array APB Clock Rate */
	SET_REG_FIELD(FCFG(FUSE_SA), FCFG_APBRT, APBclock & 0x3F);

	FUSE_IsInitialized = TRUE;

	return HAL_OK;


}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_Read                                                                              */
/*                                                                                                         */
/* Parameters:      arr  - Storage Array type [input].                                                     */
/*                  addr - Byte-address to read from [input].                                              */
/*                  data - Pointer to result [output].                                                     */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     Read 8-bit data from an OTP storage array.                                             */
/*---------------------------------------------------------------------------------------------------------*/
void FUSE_Read (FUSE_STORAGE_ARRAY_T arr,
                UINT16               addr,
                UINT8               *data
)
{
	if (!FUSE_IsInitialized)
	{
        FUSE_Init();
	}
#ifdef CONFIG_FUSE_EMULATE
     *data = fuse_array_mem[(arr*FUSE_ARR_SIZE)+addr];
#else
	/* Wait for the Fuse Box Idle */
	FUSE_WaitForOTPReadyWithTimeout(arr, 0xDEADBEEF ); /* TODO: decide proper timeout */

	/* Configure the byte address in the fuse array for read operation */
	SET_REG_FIELD(FADDR(arr), FADDR_BYTEADDR, addr);

	/* Initiate a read cycle from the byte in the fuse array, pointed by FADDR */
	REG_WRITE(FCTL(arr),  READ_INIT);

	/* Wait for read operation completion */
	FUSE_WaitForOTPReadyWithTimeout(arr, 0xDEADBEEF ); /* TODO: decide proper timeout */

	/* Read the result */
	*data = READ_REG_FIELD(FDATA(arr), FDATA_FDATA);

	/* Clean FDATA contents to prevent unauthorized software from reading sensitive information */
	SET_REG_FIELD(FDATA(arr), FDATA_FDATA, FDATA_CLEAN_VALUE);
#endif
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_BitIsProgrammed                                                                   */
/*                                                                                                         */
/* Parameters:      arr     - Storage Array type [input].                                                  */
/*                  byteNum - Byte offset in array [input].                                                */
/*                  bitNum  - Bit offset in byte [input].                                                  */
/* Returns:         Nonzero if bit is programmed, zero otherwise.                                          */
/* Side effects:                                                                                           */
/* Description:     Check if a bit is programmed in an OTP storage array.                                  */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN FUSE_BitIsProgrammed (
	FUSE_STORAGE_ARRAY_T  arr,
	UINT16                byteNum,
	UINT8                 bitNum
)
{
	UINT8 data;

	if (!FUSE_IsInitialized)
	{
        FUSE_Init();
	}

	/* Read the entire byte you wish to program */
	FUSE_Read(arr, byteNum, &data);

	/* Check whether the bit is already programmed */
	if (READ_VAR_BIT(data, bitNum))
	{
        return TRUE;
	}
	return FALSE;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_ProgramBit                                                                        */
/*                                                                                                         */
/* Parameters:      arr     - Storage Array type [input].                                                  */
/*                  byteNum - Byte offset in array [input].                                                */
/*                  bitNum  - Bit offset in byte [input].                                                  */
/* Returns:         HAL_STATUS                                                                                   */
/* Side effects:                                                                                           */
/* Description:     Program (set to 1) a bit in an OTP storage array.                                      */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_ProgramBit (
	FUSE_STORAGE_ARRAY_T arr,
	UINT16               byteNum,
	UINT8                bitNum
)
{
	HAL_STATUS status = HAL_OK;

	if (!FUSE_IsInitialized)
	{
        FUSE_Init();
	}

	/* Wait for the Fuse Box Idle */
	FUSE_WaitForOTPReadyWithTimeout(arr, 0xDEADBEEF ); /* TODO: decide proper timeout */

	/* Make sure the bit is not already programmed */
	if (! FUSE_BitIsProgrammed(arr, byteNum, bitNum))
	{

#ifdef CONFIG_FUSE_EMULATE
        fuse_array_mem[(arr*FUSE_ARR_SIZE)+byteNum] |= 1<<bitNum;
#else
        UINT8 read_data;
        int count;

        /* Configure the bit address in the fuse array for program operation */
        SET_REG_FIELD(FADDR(arr), FADDR_BYTEADDR, byteNum);

        SET_REG_FIELD(FADDR(arr), FADDR_BITPOS, bitNum);

        /* program up to MAX_PROGRAM_PULSES */
        for (count=1; count<=MAX_PROGRAM_PULSES; count++)
        {
            /* Arm the program operation */
            REG_WRITE(FCTL(arr), PROGRAM_ARM);

        /* Initiate a program cycle to the bit in the fuse array, pointed by FADDR */
        REG_WRITE(FCTL(arr), PROGRAM_INIT);

        /* Wait for program operation completion */
        FUSE_WaitForOTPReadyWithTimeout(arr, 0xDEADBEEF ); /* TODO: decide proper timeout */

            /* after MIN_PROGRAM_PULSES start verifying the result */
            if (count >= MIN_PROGRAM_PULSES)
            {
                /* Initiate a read cycle from the byte in the fuse array, pointed by FADDR */
                REG_WRITE(FCTL(arr),  READ_INIT);

                /* Wait for read operation completion */
                FUSE_WaitForOTPReadyWithTimeout(arr, 0xDEADBEEF ); /* TODO: decide proper timeout */

                /* Read the result */
                read_data = READ_REG_FIELD(FDATA(arr), FDATA_FDATA);

                /* If the bit is set the sequence ended correctly */
                if (read_data & (1 << bitNum))
                    break;
            }
        }

        /* check if programmking failed */
        if (count > MAX_PROGRAM_PULSES)
        {
            status = HAL_ERROR;
        }

        /* Clean FDATA contents to prevent unauthorized software from reading sensitive information */
        SET_REG_FIELD(FDATA(arr), FDATA_FDATA, FDATA_CLEAN_VALUE);
#endif
	}

	return status;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_ProgramByte                                                                       */
/*                                                                                                         */
/* Parameters:      arr     - Storage Array type [input].                                                  */
/*                  byteNum - Byte offset in array [input].                                                */
/*                  value   - Byte to program [input].                                                     */
/* Returns:         HAL_STATUS                                                                                   */
/* Side effects:                                                                                           */
/* Description:     Program (set to 1) a given byte's relevant bits in an OTP                              */
/*                  storage array.                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_ProgramByte (
	FUSE_STORAGE_ARRAY_T  arr,
	UINT16                byteNum,
	UINT8                 value
)
{
	UINT  i;

	UINT8 data;
	HAL_STATUS status = HAL_OK;

	if (!FUSE_IsInitialized)
	{
        FUSE_Init();
	}

	/* Wait for the Fuse Box Idle */
	FUSE_WaitForOTPReadyWithTimeout(arr, 0xDEADBEEF ); /* TODO: decide proper timeout */

	/* Read the entire byte you wish to program */
	FUSE_Read(arr, byteNum, &data);

	/* In case all relevant bits are already programmed - nothing to do */
	if ((~data & value) == 0)
        return status;

	/* Program unprogrammed bits. */
	for (i = 0; i < 8; i++)
	{
        if (READ_VAR_BIT(value, i) == 1)
        {
            /* Program (set to 1) the relevant bit */
            HAL_STATUS last_status = FUSE_ProgramBit(arr, byteNum, (UINT8)i);
            if (last_status != HAL_OK)
            {
                status = last_status;
            }
        }
	}

	return status;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_SelectKey                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  keyIndex - AES key index in the key array (in 128-bit steps) [input].                  */
/* Returns:         HAL_OK on successful read completion, HAL_STATUS_ERROR* otherwise.            */
/* Side effects:                                                                                           */
/* Description:     Select a key from the key storage array.                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_SelectKey (
	UINT8            keyIndex  )
{

	UINT32  fKeyInd = 0;
	volatile UINT32 time = 0xDAEDBEEF;

	if (!FUSE_IsInitialized)
	{
        FUSE_Init();
	}

	if (keyIndex >= 4 )
	{
        return HAL_ERROR_BAD_PARAM;
	}

	/* Do not destroy ECCDIS bit */
	fKeyInd = REG_READ(FKEYIND);

	/* Configure the key size */
	SET_VAR_FIELD(fKeyInd, FKEYIND_KSIZE, FKEYIND_KSIZE_VALUE_256);

	/* Configure the key index (0 to 3) */
	SET_VAR_FIELD(fKeyInd, FKEYIND_KIND, keyIndex);

	REG_WRITE(FKEYIND, fKeyInd);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for selection completetion                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	while (--time > 1)
	{
        if (READ_REG_FIELD(FKEYIND, FKEYIND_KVAL))
            return HAL_OK;
	}

	return HAL_ERROR;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_ReadKey                                                                           */
/*                                                                                                         */
/* Parameters:      keySize  - AES key size [input].                                                       */
/*                  keyIndex - AES key index in the key array (in 128-bit steps) [input].                  */
/*                  output   - Byte-array to hold the key [output].                                        */
/* Returns:         HAL_OK on successful read completion, HAL_ERROR_* otherwise.                   */
/* Side effects:                                                                                           */
/* Description:     Read a key from the key storage array.                                                 */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_ReadKey (
	FUSE_KEY_TYPE_T  keyType,
	UINT8            keyIndex,
	UINT8            *output)
{
	UINT  i;
	UINT16 addr = 0 ;
	UINT16 keySizeByte = 0;
	UINT8 fuseReadLock, keyBlockMask = 0;

	if (!FUSE_IsInitialized)
	{
        FUSE_Init();
	}

	/* Read the Fuse Read Lock field */
	fuseReadLock = READ_REG_FIELD(FCFG(KEY_SA), FCFG_FRDLK);

	/*AES_KEY_SIZE_256 */

	/*-----------------------------------------------------------------------------------------------------*/
	/* keyType can be either AES (4 keys, 32 bytes - AES_KEY_SIZE_256) or RSA (2 keys , 256 bytes)         */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( keyType == FUSE_KEY_AES)
	{
        keyBlockMask = 3;
        keySizeByte = AES_KEY_BYTE_SIZE(AES_256);
	}
	else if (keyType == FUSE_KEY_RSA)
	{
        keyBlockMask = 3;
        keySizeByte = RSA_KEY_BYTE_SIZE;
	}

	/* Build the key block mask based on key index */
	keyBlockMask <<= keyIndex;

	/* Verify that APB Read Access to the Key Storage Array is allowed */
	if (READ_REG_FIELD(FUSTRAP, FUSTRAP_oAESKEYACCLK) ||
        READ_VAR_MASK(fuseReadLock, keyBlockMask))
	{
        return HAL_ERROR_LOCKED;
	}

	/* Calculate key size in bytes */
	/*TODO:   keySizeByte = AES_KEY_BYTE_SIZE(keySize); */

	/* Calculate key start address in Key Storage Array -
       Note that a key always starts on a 128-bit (i.e., a complete block) boundary */
	/*TODO:   addr = keyIndex * AES_BLOCK_SIZE; */

	/* Read Key */
	for (i = 0; i < keySizeByte; i++, addr++)
	{
        FUSE_Read(KEY_SA, addr, &output[i]);
	}
	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_DisableKeyAccess                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disables read and write accees to key array                               */
/*---------------------------------------------------------------------------------------------------------*/
void FUSE_DisableKeyAccess ()
{
	if (!FUSE_IsInitialized)
	{
        FUSE_Init();
	}
	DISABLE_KEY_ACCESS();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_LockAccess                                                                        */
/*                                                                                                         */
/* Parameters:      lockForRead: bitwise, which block to lock for reading                                  */
/* Parameters:      lockForWrite: bitwise, which block to lock for program                                 */
/* Returns:         HAL_STATUS                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine lock the otp blocks                                                       */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_LockAccess (FUSE_STORAGE_ARRAY_T array, UINT8 lockForRead, UINT8 lockForWrite, BOOLEAN lockRegister)
{
	UINT32 FCFG_VAR = 0;

	if (!FUSE_IsInitialized)
	{
        FUSE_Init();
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* check parameters validity                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	if (array > FUSE_SA)
	{
        return HAL_ERROR_BAD_PARAM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read reg for modify all fields apart APBRT                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	FCFG_VAR = REG_READ(FCFG(array));


	SET_VAR_FIELD(FCFG_VAR, FCFG_FRDLK,  lockForRead & 0x00FF);

	SET_VAR_FIELD(FCFG_VAR, FCFG_FPRGLK, lockForWrite & 0x00FF);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Lock any access to this register (until next POR)                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( lockRegister == TRUE)
	{
        SET_VAR_FIELD(FCFG_VAR, FCFG_FCFGLK, (lockForWrite | lockForRead) & 0x00FF);
	}

     /*----------------------------------------------------------------------------------------------------*/
     /* Lock the side band in case it's a key array, and read is locked                                    */
     /*----------------------------------------------------------------------------------------------------*/
	if ( array == KEY_SA)
	{
        /* Set FDIS bit if oKAP bit 7 is set, to disable the side-band key loading. */
        if ( (lockForRead & 0x80) > 0 )
        {
            SET_VAR_FIELD(FCFG_VAR, FCFG_FDIS, 1);  /* 1: Access to the first 2048 bits of the fuse array is disabled. */
        }
	}

#ifdef _DEBUG_
	if (array == KEY_SA)
	{
        *(UINT32*)(PCIMBX_PHYS_BASE_ADDR+0x10)  = FCFG_VAR;                /* ROM_MAILBOX_DEBUG9 */
        *(UINT32*)(PCIMBX_PHYS_BASE_ADDR+0x14)  = (UINT32)lockForWrite;    /* ROM_MAILBOX_DEBUG10 */
	}
	else if (array == FUSE_SA)
	{
        *(UINT32*)(PCIMBX_PHYS_BASE_ADDR+0x18)  = FCFG_VAR;                /* ROM_MAILBOX_DEBUG11 */
        *(UINT32*)(PCIMBX_PHYS_BASE_ADDR+0x1C)  = (UINT32)lockForWrite;    /* ROM_MAILBOX_DEBUG12 */
	}
	else
	{
        *(UINT32*)(PCIMBX_PHYS_BASE_ADDR+0x18)  = FCFG_VAR;                /* ROM_MAILBOX_DEBUG11 */
        *(UINT32*)(PCIMBX_PHYS_BASE_ADDR+0x1C)  = 0xDEADDEAD;              /* ROM_MAILBOX_DEBUG12 */
	}
#endif

	/*-----------------------------------------------------------------------------------------------------*/
	/* Return the moified value                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(FCFG(array), FCFG_VAR);

	return HAL_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Logical level functions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_NibParEccDecode                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  datain -       pointer to encoded data buffer (buffer size should be 2 x dataout)      */
/*                  dataout -      pointer to decoded data buffer                                          */
/*                  encoded_size - size of encoded data (decoded data x 2)                                 */
/*                                                                                                         */
/* Returns:         HAL_STATUS                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Decodes the data according to nibble parity ECC scheme.                                */
/*                  Size specifies the encoded data size.                                                  */
/*                  Decodes whole bytes only                                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_NibParEccDecode (
	UINT8  *datain,
	UINT8  *dataout,
	UINT32  encoded_size
)
{
	UINT32 i;
	UINT8 BER;
	UINT8 src_re_calc;
	UINT8 dst;
	UINT8 E0;
	UINT8 E1;
	UINT8 E2;
	UINT8 E3;
	UINT8 E4;
	UINT8 E5;
	UINT8 E6;
	UINT8 E7;
	HAL_STATUS status = HAL_OK;

/*Define the Bit Field macros in order to use the SET_VAR_FIELD macro: */
#define BITF0   0, 1
#define BITF1   1, 1
#define BITF2   2, 1
#define BITF3   3, 1
#define BITF4   4, 1
#define BITF5   5, 1
#define BITF6   6, 1
#define BITF7   7, 1

#define LSNF    0, 4
#define MSNF    4, 4

	for (i = 0; i < encoded_size; i++)
	{
        E0 = READ_VAR_FIELD(datain[i], BITF0);
        E1 = READ_VAR_FIELD(datain[i], BITF1);
        E2 = READ_VAR_FIELD(datain[i], BITF2);
        E3 = READ_VAR_FIELD(datain[i], BITF3);
        E4 = READ_VAR_FIELD(datain[i], BITF4);
        E5 = READ_VAR_FIELD(datain[i], BITF5);
        E6 = READ_VAR_FIELD(datain[i], BITF6);
        E7 = READ_VAR_FIELD(datain[i], BITF7);

        if (i % 2)
        {/*Decode higher nibble */
            SET_VAR_FIELD(dataout[i/2], BITF4, ((E0 & (E1 ^ E4)) | (E0 & (E2 ^ E6)) | ((E1 ^ E4) & (E2 ^ E6))));
            SET_VAR_FIELD(dataout[i/2], BITF5, ((E1 & (E0 ^ E4)) | (E1 & (E3 ^ E7)) | ((E0 ^ E4) & (E3 ^ E7))));
            SET_VAR_FIELD(dataout[i/2], BITF6, ((E2 & (E0 ^ E6)) | (E2 & (E3 ^ E5)) | ((E0 ^ E6) & (E3 ^ E5))));
            SET_VAR_FIELD(dataout[i/2], BITF7, ((E3 & (E2 ^ E5)) | (E3 & (E1 ^ E7)) | ((E2 ^ E5) & (E1 ^ E7))));

            dst = MSN( dataout[i/2] );
        }
        else
        {/*Decode lower nibble */
            SET_VAR_FIELD(dataout[i/2], BITF0, ((E0 & (E1 ^ E4)) | (E0 & (E2 ^ E6)) | ((E1 ^ E4) & (E2 ^ E6))));
            SET_VAR_FIELD(dataout[i/2], BITF1, ((E1 & (E0 ^ E4)) | (E1 & (E3 ^ E7)) | ((E0 ^ E4) & (E3 ^ E7))));
            SET_VAR_FIELD(dataout[i/2], BITF2, ((E2 & (E0 ^ E6)) | (E2 & (E3 ^ E5)) | ((E0 ^ E6) & (E3 ^ E5))));
            SET_VAR_FIELD(dataout[i/2], BITF3, ((E3 & (E2 ^ E5)) | (E3 & (E1 ^ E7)) | ((E2 ^ E5) & (E1 ^ E7))));

            dst = LSN( dataout[i/2] );
        }


        /*-------------------------------------------------------------------------------------------------*/
        /* calculate the encoded value back from the decoded value and compare the original value for      */
        /* comparison                                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
		/* Take decode byte*/
        src_re_calc = dst;

		/* calc its' parity */
        E0 = READ_VAR_FIELD(dst, BITF0);
        E1 = READ_VAR_FIELD(dst, BITF1);
        E2 = READ_VAR_FIELD(dst, BITF2);
        E3 = READ_VAR_FIELD(dst, BITF3);

        SET_VAR_FIELD(src_re_calc, BITF4, E0 ^ E1);
        SET_VAR_FIELD(src_re_calc, BITF5, E2 ^ E3);
        SET_VAR_FIELD(src_re_calc, BITF6, E0 ^ E2);
        SET_VAR_FIELD(src_re_calc, BITF7, E1 ^ E3);

        /*-----------------------------------------------------------------------------------------------------*/
        /* Check that only one bit is corrected per byte                                                       */
        /*-----------------------------------------------------------------------------------------------------*/
        BER = src_re_calc ^ datain[i];

        BER =    READ_VAR_FIELD(BER, BITF0)
               + READ_VAR_FIELD(BER, BITF1)
               + READ_VAR_FIELD(BER, BITF2)
               + READ_VAR_FIELD(BER, BITF3)
               + READ_VAR_FIELD(BER, BITF4)
               + READ_VAR_FIELD(BER, BITF5)
               + READ_VAR_FIELD(BER, BITF6)
               + READ_VAR_FIELD(BER, BITF7);

        /*-------------------------------------------------------------------------------------------------*/
        /* Bit Error Rate can be 0x00 (no change) or 0x01 0x02 0x04 0x08 -> one bit change only            */
        /*-------------------------------------------------------------------------------------------------*/
        if ( BER > 1 )
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Use original nible :                                                                        */
            /*---------------------------------------------------------------------------------------------*/
            if (i % 2)
            { /* copy lower nibble to higher nibble */
                SET_VAR_FIELD(dataout[i/2], MSNF, LSN( datain[i] ));

            }
            else
            { /* copy lower nibble to lower nibble */
                SET_VAR_FIELD(dataout[i/2], LSNF, LSN( datain[i] ) );
            }

            status = HAL_ERROR_BAD_PARITY;
        }


	}

	return status;



}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_NibParEccEncode                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  datain -       pointer to decoded data buffer (buffer size should be 2 x dataout)      */
/*                  dataout -      pointer to encoded data buffer                                          */
/*                  encoded_size - size of encoded data (decoded data x 2)                                 */
/*                                                                                                         */
/* Returns:         HAL_STATUS                                                                             */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Decodes the data according to nibble parity ECC scheme.                                */
/*                  Size specifies the encoded data size.                                                  */
/*                  Decodes whole bytes only                                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_NibParEccEncode (
	UINT8 *datain,
	UINT8 *dataout,
	UINT32 encoded_size
)
{
	UINT32 i;
	UINT8 E0;
	UINT8 E1;
	UINT8 E2;
	UINT8 E3;
	HAL_STATUS status = HAL_OK;
	UINT32 decoded_size = encoded_size/2;

/*Define the Bit Field macros in order to use the SET_VAR_FIELD macro: */
#define BITF0   0, 1
#define BITF1   1, 1
#define BITF2   2, 1
#define BITF3   3, 1
#define BITF4   4, 1
#define BITF5   5, 1
#define BITF6   6, 1
#define BITF7   7, 1

#define LSNF    0, 4
#define MSNF    4, 4

	for (i = 0; i < decoded_size; i++)
	{
        dataout[i*2] = LSN(datain[i]);
        E0 = READ_VAR_FIELD(datain[i], BITF0);
        E1 = READ_VAR_FIELD(datain[i], BITF1);
        E2 = READ_VAR_FIELD(datain[i], BITF2);
        E3 = READ_VAR_FIELD(datain[i], BITF3);

        SET_VAR_FIELD(dataout[i*2], BITF4, E0 ^ E1);
        SET_VAR_FIELD(dataout[i*2], BITF5, E2 ^ E3);
        SET_VAR_FIELD(dataout[i*2], BITF6, E0 ^ E2);
        SET_VAR_FIELD(dataout[i*2], BITF7, E1 ^ E3);

        dataout[i*2+1] = MSN(datain[i]);
        E0 = READ_VAR_FIELD(datain[i], BITF4);
        E1 = READ_VAR_FIELD(datain[i], BITF5);
        E2 = READ_VAR_FIELD(datain[i], BITF6);
        E3 = READ_VAR_FIELD(datain[i], BITF7);

        SET_VAR_FIELD(dataout[i*2+1], BITF4, E0 ^ E1);
        SET_VAR_FIELD(dataout[i*2+1], BITF5, E2 ^ E3);
        SET_VAR_FIELD(dataout[i*2+1], BITF6, E0 ^ E2);
        SET_VAR_FIELD(dataout[i*2+1], BITF7, E1 ^ E3);
	}

	return status;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_MajRulEccDecode                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  datain -       pointer to encoded data buffer (buffer size should be 3 x dataout)      */
/*                  dataout -      pointer to decoded data buffer                                          */
/*                  encoded_size - size of encoded data (decoded data x 3)                                 */
/*                                                                                                         */
/* Returns:         HAL_STATUS                                                                             */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Decodes the data according to Major Rule ECC scheme.                                   */
/*                  Size specifies the encoded data size.                                                  */
/*                  Decodes whole bytes only                                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_MajRulEccDecode (
	UINT8 *datain,
	UINT8 *dataout,
	UINT32 encoded_size
)
{
	UINT  byte;
	UINT  bit;
	UINT8 E1, E2, E3;

	UINT32 decoded_size = encoded_size/3;

	for (byte = 0; byte < decoded_size; byte++)
	{
        for (bit = 0; bit < 8; bit++)
        {
            E1 = READ_VAR_BIT(datain[decoded_size*0+byte], bit);
            E2 = READ_VAR_BIT(datain[decoded_size*1+byte], bit);
            E3 = READ_VAR_BIT(datain[decoded_size*2+byte], bit);
            if ((E1+E2+E3) >= 2)
            {
                SET_VAR_BIT(dataout[byte], bit);    /*Majority is 1 */
            }
            else
            {
                 CLEAR_VAR_BIT(dataout[byte], bit); /*Majority is 0 */
            }
        }/*Inner for (bit) */
	}/*Outer for (byte) */

	return HAL_OK;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_MajRulEccEncode                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  datain -       pointer to decoded data buffer (buffer size should be 3 x dataout)      */
/*                  dataout -      pointer to encoded data buffer                                          */
/*                  encoded_size - size of encoded data (decoded data x 3)                                 */
/*                                                                                                         */
/* Returns:         HAL_STATUS                                                                             */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Decodes the data according to Major Rule ECC scheme.                                   */
/*                  Size specifies the encoded data size.                                                  */
/*                  Decodes whole bytes only                                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS FUSE_MajRulEccEncode (
	UINT8 *datain,
	UINT8 *dataout,
	UINT32 encoded_size
)
{
	UINT  byte;
	UINT  bit;
	UINT8 bit_val;

	UINT32 decoded_size = encoded_size/3;

	for (byte = 0; byte < decoded_size; byte++)
	{
        for (bit = 0; bit < 8; bit++)
        {
            bit_val = READ_VAR_BIT(datain[byte], bit);

            if (bit_val == 1)
            {
                SET_VAR_BIT(dataout[decoded_size*0+byte], bit);
                SET_VAR_BIT(dataout[decoded_size*1+byte], bit);
                SET_VAR_BIT(dataout[decoded_size*2+byte], bit);

            }
            else
            {
                CLEAR_VAR_BIT(dataout[decoded_size*0+byte], bit);
                CLEAR_VAR_BIT(dataout[decoded_size*1+byte], bit);
                CLEAR_VAR_BIT(dataout[decoded_size*2+byte], bit);
            }
        } /* Inner for (bit) */
	}/* Outer for (byte) */

	return HAL_OK;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_ReadFustrap                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  oFuse - fuse value to read                                                             */
/*                                                                                                         */
/* Returns:         retVal                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This is a getter for fustrap                                                           */
/*---------------------------------------------------------------------------------------------------------*/
UINT           FUSE_Fustrap_Get (FUSE_FUSTRAP_FIELDS_T oFuse)
{
	UINT retVal = 0;
	switch (oFuse)
	{
        case FUSE_FUSTRAP_DIS_FAST_BOOT:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_DIS_FAST_BOOT);
            break;

        case FUSE_FUSTRAP_oWDEN:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oWDEN);
            break;

        case FUSE_FUSTRAP_oHLTOF:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oHLTOF);
            break;

        case FUSE_FUSTRAP_oAESKEYACCLK:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oAESKEYACCLK);
            break;

        case FUSE_FUSTRAP_oJDIS:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oJDIS);
            break;

        case FUSE_FUSTRAP_oSECBOOT:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oSECBOOT);
            break;

        case FUSE_FUSTRAP_USEFUSTRAP:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_USEFUSTRAP);
            break;

        case FUSE_FUSTRAP_oPKInvalid2_0:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oPKInvalid2_0);
            break;

        case FUSE_FUSTRAP_oAltImgLoc:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oAltImgLoc);
            break;

        case FUSE_FUSTRAP_Bit_28:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_BIT_28);
            break;

        case FUSE_FUSTRAP_oSecBootDisable:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oSecBootDisable);
            break;

        case FUSE_FUSTRAP_oCPU1STOP2:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oCPU1STOP2);
            break;

        case FUSE_FUSTRAP_oCPU1STOP1:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oCPU1STOP1);
            break;

        case FUSE_FUSTRAP_oHINDDIS:
            retVal = READ_REG_FIELD(FUSTRAP, FUSTRAP_oHINDDIS);
            break;


        default:
            ASSERT(FALSE);
            break;
	}

	return retVal;
}


