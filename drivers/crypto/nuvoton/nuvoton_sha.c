/*---------------------------------------------------------------------------------------------------------*/
/*	Nuvoton Technology Corporation Confidential 														   */
/*																										   */
/*	Copyright (c)      2016 by Nuvoton Technology Corporation											   */
/*	All rights reserved 																				   */
/*																										   */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:																						   */
/*	 nuvoton_sha.c																						   */
/*			  This file contains HW IF to Nuvoton BMC SHA SHA engine            				    	   */
/*---------------------------------------------------------------------------------------------------------*/


#ifdef CONFIG_SHA_HW_ACCEL

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <mapmem.h>
#include <hw_sha.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <config.h>


#include <hash.h>
#include <u-boot/crc.h>
#include <u-boot/sha1.h>
#include <u-boot/sha256.h>
#include <u-boot/md5.h>


#include "nuvoton_sha.h"


/* #define SHA_PRINT 1 */
#define CONFIG_PARALLEL_SHA

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                       LOCAL FUNCTIONS DECLARATION                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

static void                             SHA_FlushLocalBuffer_l (const UINT32* buff);
static int                              SHA_BusyWait_l (void);
static void                             SHA_GetShaDigest_l (UINT8* hashDigest, SHA_TYPE_T shaType);
static void                             SHA_SetShaDigest_l (const UINT32* hashDigest, SHA_TYPE_T shaType);
static void                             SHA_SetBlock_l (const UINT8* data,UINT32 len, UINT16 position, UINT32* block);
static void                             SHA_ClearBlock_l (UINT16 len, UINT16 position, UINT32* block);
static void                             SHA_SetLength32_l (const SHA_HANDLE_T* handlePtr, UINT32* block);



extern void Tick (void);
extern void Tock (void);



/*
	*          static functions
	*/


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           TYPES & DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#define SHA_SECRUN_BUFF_SIZE            64
#define SHA_TIMEOUT                     100
#define SHA_DATA_LAST_BYTE              0x80
/* #define SHA_SET_TYPE(type)              SET_REG_FIELD(HASH_CFG, HASH_CFG_SHA1_SHA2, type) */

#define SHA_SET_TYPE(type)                                                                                        \
         (type == SHA_TYPE_SHA2) ?                                                                                \
            writeb(readb(&sha_dev->HASH_CFG) & ~HASH_CFG_SHA1_SHA2, &sha_dev->HASH_CFG)  :   \
            writeb(readb(&sha_dev->HASH_CFG) | HASH_CFG_SHA1_SHA2, &sha_dev->HASH_CFG)


#ifdef SHA_SELF_TEST
#define SHA2_NUM_OF_SELF_TESTS          3
#define SHA1_NUM_OF_SELF_TESTS          4
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           SHA_BUFF_POS                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  length - number of bytes written                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine compute the # of bytes currently in the sha block buffer                  */
/*---------------------------------------------------------------------------------------------------------*/
#define SHA_BUFF_POS(length)            (length & (SHA_BLOCK_LENGTH - 1))

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           SHA_BUFF_FREE                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  length - number of bytes written                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine compute the # of free bytes in the sha block buffer                       */
/*---------------------------------------------------------------------------------------------------------*/
#define SHA_BUFF_FREE(length)           (SHA_BLOCK_LENGTH - SHA_BUFF_POS(length))

/*---------------------------------------------------------------------------------------------------------*/
/* Macro:           SHA_RET_CHECK                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func    - function to check                                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This macro checks if give function returns int error, and returns the error    */
/*                  immediately after SHA disabling                                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define SHA_RET_CHECK(func)                                         \
{                                                                   \
	int status;                                                     \
                                                                    \
	if ((status = func) != 0)                                       \
	{                                                               \
       DEFS_STATUS_RET_CHECK(SHA_Power(false));                     \
       return status;                                               \
	}                                                               \
}



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Calc                                                                               */
/*                                                                                                         */
/* Parameters:      shaType     - SHA module type                                                          */
/*                  inBuff      -    Pointer to a buffer containing the data to be hashed                  */
/*                  len         -    Length of the data to hash                                            */
/*                  hashDigest  -   Pointer to a buffer where the reseulting digest will be copied to      */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success or other int error code on error                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs complete SHA calculation in one step including SHA_Init routine  */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_Calc (SHA_TYPE_T shaType, const UINT8* inBuff, UINT32 len, UINT8* hashDigest);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Init                                                                               */
/*                                                                                                         */
/* Parameters:      handlePtr - SHA processing handle pointer                                              */
/* Returns:         DEFS_STATUS_OK on success or other int error code on error.                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialize the SHA module                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_Init (SHA_HANDLE_T* handlePtr);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Start                                                                              */
/*                                                                                                         */
/* Parameters:      handlePtr   - SHA processing handle pointer                                            */
/*                  shaType     - SHA module type                                                          */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success or other int error code on error.                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine start a single SHA process                                                */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_Start (SHA_HANDLE_T* handlePtr, SHA_TYPE_T shaType);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Update                                                                             */
/*                                                                                                         */
/* Parameters:      handlePtr  -   SHA processing handle pointer                                           */
/*                  buffer -   Pointer to the data that will be added to the hash calculation              */
/*                  len -      Length of data to add to SHA calculation                                    */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success or other int error code on error                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine adds data to previously started SHA calculation                           */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_Update (SHA_HANDLE_T* handlePtr, const UINT8* buffer, UINT32 len);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Finish                                                                             */
/*                                                                                                         */
/* Parameters:      handlePtr  -   SHA processing handle pointer                                           */
/*                  hashDigest -     Pointer to a buffer where the final digest will be copied to          */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success or other int error code on error                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine finish SHA calculation and get the resulting SHA digest                   */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_Finish (SHA_HANDLE_T* handlePtr, UINT8* hashDigest);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Reset                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reset SHA module                                                          */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_Reset (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Power                                                                              */
/*                                                                                                         */
/* Parameters:      on - TRUE enable the module, FALSE disable the module                                  */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set SHA module power on/off                                               */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_Power (bool on);

#ifdef SHA_PRINT
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
static void SHA_PrintRegs (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_PrintVersion                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static void SHA_PrintVersion (void);
#endif

#ifdef SHA_SELF_TEST
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_SelfTest                                                                           */
/*                                                                                                         */
/* Parameters:      shaType - SHA module type                                                              */
/* Returns:         DEFS_STATUS_OK on success or other int error code on error                             */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs various tests on the SHA HW and SW                               */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_SelfTest (SHA_TYPE_T shaType);
#endif /* SHA_SELF_TEST */


/*---------------------------------------------------------------------------------------------------------*/
/* Static                                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
static SHA_HANDLE_T sha_handle;

static volatile nuvoton_bmc_sha *sha_dev = (nuvoton_bmc_sha *)SHA_BASE_ADDR;






/**
	* Computes hash value of input pbuf using h/w acceleration
	*
	* @param in_addr	A pointer to the input buffer
	* @param bufleni	Byte length of input buffer
	* @param out_addr	A pointer to the output buffer. When complete
	*			32 bytes are copied to pout[0]...pout[31]. Thus, a user
	*			should allocate at least 32 bytes at pOut in advance.
	* @param chunk_size	chunk size for sha256
	*/
void hw_sha256(const uchar * in_addr, uint buflen,
			uchar * out_addr, uint chunk_size)
{
	puts("\nhw_sha256 using BMC HW accelerator\t\t");
	SHA_Calc(SHA_TYPE_SHA2, (UINT8 *)in_addr, buflen, (UINT8 *)out_addr);
	return;
}

/**
	* Computes hash value of input pbuf using h/w acceleration
	*
	* @param in_addr	A pointer to the input buffer
	* @param bufleni	Byte length of input buffer
	* @param out_addr	A pointer to the output buffer. When complete
	*			32 bytes are copied to pout[0]...pout[31]. Thus, a user
	*			should allocate at least 32 bytes at pOut in advance.
	* @param chunk_size	chunk_size for sha1
	*/
void hw_sha1(const uchar * in_addr, uint buflen,
			uchar * out_addr, uint chunk_size)
{
	puts("\nhw_sha1 using BMC HW accelerator\t\t");
	SHA_Calc(SHA_TYPE_SHA1, (UINT8 *)in_addr, buflen, (UINT8 *)out_addr);
	return;
}

/*
	* Create the context for sha progressive hashing using h/w acceleration
	*
	* @algo: Pointer to the hash_algo struct
	* @ctxp: Pointer to the pointer of the context for hashing
	* @return 0 if ok, -ve on error
	*/
int hw_sha_init(struct hash_algo *algo, void **ctxp)
{
	return SHA_Init(&sha_handle);

}

/*
	* Update buffer for sha progressive hashing using h/w acceleration
	*
	* The context is freed by this function if an error occurs.
	*
	* @algo: Pointer to the hash_algo struct
	* @ctx: Pointer to the context for hashing
	* @buf: Pointer to the buffer being hashed
	* @size: Size of the buffer being hashed
	* @is_last: 1 if this is the last update; 0 otherwise
	* @return 0 if ok, -ve on error
	*/
int hw_sha_update(struct hash_algo *algo, void *ctx, const void *buf,
		     unsigned int size, int is_last)
{
	return SHA_Update(&sha_handle, buf, size);
}

/*
	* Copy sha hash result at destination location
	*
	* The context is freed after completion of hash operation or after an error.
	*
	* @algo: Pointer to the hash_algo struct
	* @ctx: Pointer to the context for hashing
	* @dest_buf: Pointer to the destination buffer where hash is to be copied
	* @size: Size of the buffer being hashed
	* @return 0 if ok, -ve on error
	*/
int hw_sha_finish(struct hash_algo *algo, void *ctx, void *dest_buf,
		     int size)
{
	return SHA_Finish(&sha_handle, dest_buf);
}






/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERNAL  FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Calc                                                                               */
/*                                                                                                         */
/* Parameters:      shaType - SHA module type                                                              */
/*                  inBuff -    Pointer to a buffer containing the data to be hashed                       */
/*                  len -    Length of the data to hash                                                    */
/*                  hashDigest -   Pointer to a buffer where the reseulting digest will be copied to       */
/*                                                                                                         */
/* Returns:         0 on success or other int error code on error                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs complete SHA calculation in one step                             */
/*---------------------------------------------------------------------------------------------------------*/
int SHA_Calc (
	SHA_TYPE_T shaType,
	const UINT8* inBuff,
	UINT32 len,
	UINT8* hashDigest
)
{
	SHA_HANDLE_T handle;

	DEFS_STATUS_RET_CHECK(SHA_Init(&handle));
	DEFS_STATUS_RET_CHECK(SHA_Power(TRUE));
	SHA_RET_CHECK(SHA_Reset());
	SHA_RET_CHECK(SHA_Start(&handle, shaType));
	SHA_RET_CHECK(SHA_Update(&handle, inBuff, len));
	SHA_RET_CHECK(SHA_Finish(&handle, hashDigest));
#ifndef CONFIG_PARALLEL_SHA
	SHA_GetShaDigest_l(hashDigest, shaType);
#endif /* CONFIG_PARALLEL_SHA */
	DEFS_STATUS_RET_CHECK(SHA_Power(false));
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Init                                                                               */
/*                                                                                                         */
/* Parameters:      handlePtr - SHA processing handle pointer                                              */
/* Returns:         0 on success or other int error code on error.                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialize the SHA module                                                 */
/*---------------------------------------------------------------------------------------------------------*/
int SHA_Init (SHA_HANDLE_T* handlePtr)
{
	handlePtr->active = false;

	/* SHA_PrintVersion(); */

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Start                                                                              */
/*                                                                                                         */
/* Parameters:      handlePtr   - SHA processing handle pointer                                            */
/*                  shaType     - SHA module type                                                          */
/*                                                                                                         */
/* Returns:         0 on success or other int error code on error.                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine start a single SHA process                                                */
/*---------------------------------------------------------------------------------------------------------*/
int SHA_Start (
	SHA_HANDLE_T* handlePtr,
	SHA_TYPE_T shaType
)
{
	UINT16 l;



	/*-----------------------------------------------------------------------------------------------------*/
	/* Initialize handle                                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	handlePtr->length0 = 0;
	handlePtr->length1 = 0;
	handlePtr->shaType = shaType;
	handlePtr->active = TRUE;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Reset SHA hardware                                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	/* SET_REG_FIELD(HASH_CTR_STS, HASH_CTR_STS_SHA_RST, 0x01); */
	SHA_Reset();

	/*-----------------------------------------------------------------------------------------------------*/
	/* The handlePtr->hv is initialized with the correct IV as the SHA engine automaticly                  */
	/* fill the HASH_DIG_Hn registers according to SHA spec (following SHA_RST assertion)                  */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_GetShaDigest_l((UINT8*)handlePtr->hv, shaType);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Init block with zeros                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	for(l = 0 ; l < (SHA_BLOCK_LENGTH / sizeof(UINT32)); l++)
	{
        handlePtr->block[l] = 0;
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Update                                                                             */
/*                                                                                                         */
/* Parameters:      handlePtr  -   SHA processing handle pointer                                           */
/*                  buffer -   Pointer to the data that will be added to the hash calculation              */
/*                  len -      Length of data to add to SHA calculation                                    */
/*                                                                                                         */
/*                                                                                                         */
/* Returns:         0 on success or other int error code on error                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine adds data to previously started SHA calculation                           */
/*---------------------------------------------------------------------------------------------------------*/
int SHA_Update (
	SHA_HANDLE_T* handlePtr,
	const UINT8* buffer,
	UINT32 len
)
{
#ifdef CONFIG_PARALLEL_SHA
	UINT32 localBuffer[SHA_SECRUN_BUFF_SIZE / sizeof(UINT32)];
	UINT8* blockPtr;
#else
		UINT32 i;
	UINT8  len_sha_block = ((handlePtr->shaType == SHA_TYPE_SHA2) ? SHA_BLOCK_LENGTH : 10);

#endif
	UINT32 bufferLen          = len;
	UINT32 pos                = 0;



	/*-----------------------------------------------------------------------------------------------------*/
	/* Error check                                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_COND_CHECK(handlePtr->active == TRUE, DEFS_STATUS_SYSTEM_NOT_INITIALIZED);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait till SHA is not busy                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_RET_CHECK(SHA_BusyWait_l());

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set SHA type                                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_SET_TYPE(handlePtr->shaType);

	/* SHA_PrintRegs(); */


#ifdef CONFIG_PARALLEL_SHA

	/*-----------------------------------------------------------------------------------------------------*/
	/* Write SHA latest digest into SHA module                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_SetShaDigest_l(handlePtr->hv, handlePtr->shaType);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set number of unhashed bytes which remained from last update                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	pos = SHA_BUFF_POS(handlePtr->length0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Copy unhashed bytes which remained from last update to secrun buffer                                */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_SetBlock_l((UINT8*)handlePtr->block, pos, 0, localBuffer);

	while (len)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Wait for the hardware to be available (in case we are hashing)                                  */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(SHA_BusyWait_l());

        /*-------------------------------------------------------------------------------------------------*/
        /* Move as much bytes  as we can into the secrun buffer                                            */
        /*-------------------------------------------------------------------------------------------------*/
        bufferLen = (len < SHA_BUFF_FREE(handlePtr->length0)) ? len : SHA_BUFF_FREE(handlePtr->length0);

        /*-------------------------------------------------------------------------------------------------*/
        /* Copy current given buffer to the secrun buffer                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        SHA_SetBlock_l((UINT8*)buffer, bufferLen, pos, localBuffer);

        /*-------------------------------------------------------------------------------------------------*/
        /* Update size of hashed bytes                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        handlePtr->length0 += bufferLen;

        if ((handlePtr->length0) < bufferLen)
        {
            handlePtr->length1++;
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Update length of data left to digest                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        len -= bufferLen;

        /*-------------------------------------------------------------------------------------------------*/
        /* Update given buffer pointer                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        buffer += bufferLen;

        /*-------------------------------------------------------------------------------------------------*/
        /* If secrun buffer is full                                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        if (SHA_BUFF_POS(handlePtr->length0) == 0)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* We just filled up the buffer perfectly, so let it hash                                      */
            /* (we'll unload the hash only when we are done with all hashing)                              */
            /*---------------------------------------------------------------------------------------------*/
            SHA_FlushLocalBuffer_l(localBuffer);

            pos = 0;
            bufferLen = 0;
        }
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait till SHA is not busy                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_RET_CHECK(SHA_BusyWait_l());

	/*-----------------------------------------------------------------------------------------------------*/
	/* Copy unhashed bytes from given buffer to handle block for next update/finish                        */
	/*-----------------------------------------------------------------------------------------------------*/
	blockPtr = (UINT8*)handlePtr->block;
	while (bufferLen)
	{
        blockPtr[--bufferLen+pos] = *(--buffer);
	}


#else

	pos = 0;

	while (len)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Move as much bytes  as we can into the secrun buffer                                            */
        /*-------------------------------------------------------------------------------------------------*/
        bufferLen = (len < len_sha_block) ? len : len_sha_block;

        /*-------------------------------------------------------------------------------------------------*/
        /* Copy current given buffer to the secrun buffer                                                  */
        /*-------------------------------------------------------------------------------------------------*/
		for(i = 0; i < bufferLen; i+=4)
		{
			writel ( *(UINT32*)(buffer+pos + i) , &sha_dev->HASH_DATA_IN);
			/*printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", */
			/*	(UINT32*)(buffer+pos + i), pos , bufferLen, *(UINT32*)(buffer+pos + i)); */
		}
		if ( len < len_sha_block)
		{
			for(i = len; i < bufferLen; i+=4)
			{
				writel ( 0 , &sha_dev->HASH_DATA_IN);
			}
		}

#if SHA_TRY_OPTIMIZE
		i= 0;
		if ( bufferLen == len_sha_block)
		{
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
			writel ( *(UINT32*)(buffer+pos + 4*i++) , &sha_dev->HASH_DATA_IN);  /*  printf("buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n", (UINT32*)(buffer+pos + 4*i), pos , bufferLen, *(UINT32*)(buffer+pos + 4*i)); */
		}
		else
		{
			for(i = 0; i < bufferLen; i+=4)
			{
		    	writel ( *(UINT32*)(buffer+pos + i) , &sha_dev->HASH_DATA_IN);
		    	printf("for loop buffer = 0x%x, pos = %x, bufferLen= %x, data = 0x%x\n",
		    		(UINT32*)(buffer+pos + i), pos , bufferLen, *(UINT32*)(buffer+pos + i));
		    }
		}
#endif /* SHA_TRY_OPTIMIZE */

        /*-------------------------------------------------------------------------------------------------*/
        /* Update length of data left to digest                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        len -= bufferLen;
		pos += bufferLen;

		/*-------------------------------------------------------------------------------------------------*/
        /* Update size of hashed bytes                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        handlePtr->length0 += bufferLen;

        if ((handlePtr->length0) < bufferLen)
        {
            handlePtr->length1++;
        }


        /*-----------------------------------------------------------------------------------------------------*/
        /* Wait till SHA is not busy                                                                           */
        /*-----------------------------------------------------------------------------------------------------*/
        /* DEFS_STATUS_RET_CHECK(SHA_BusyWait_l()); */
        while((readb(&(sha_dev->HASH_CTR_STS)) & 0x02) == 0x02);



	}

#endif


	/* SHA_PrintRegs(); */


	/*-----------------------------------------------------------------------------------------------------*/
	/* Save SHA current digest                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_GetShaDigest_l((UINT8*)handlePtr->hv, handlePtr->shaType);

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Finish                                                                             */
/*                                                                                                         */
/* Parameters:      handlePtr  -   SHA processing handle pointer                                           */
/*                  hashDigest -     Pointer to a buffer where the final digest will be copied to          */
/*                                                                                                         */
/* Returns:         0 on success or other int error code on error                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine finish SHA calculation and get the resulting SHA digest                   */
/*---------------------------------------------------------------------------------------------------------*/
int SHA_Finish (
	SHA_HANDLE_T* handlePtr,
	UINT8* hashDigest

)
{
	UINT32 localBuffer[SHA_SECRUN_BUFF_SIZE / sizeof(UINT32)];
	const UINT8 lastbyte = SHA_DATA_LAST_BYTE;
	UINT16 pos;

	/*SHA_PrintRegs(); */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Error check                                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_COND_CHECK(handlePtr->active == TRUE, DEFS_STATUS_SYSTEM_NOT_INITIALIZED);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set SHA type                                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_SET_TYPE(handlePtr->shaType);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait till SHA is not busy                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_RET_CHECK(SHA_BusyWait_l());

	/*-----------------------------------------------------------------------------------------------------*/
	/* Finish off the current buffer with the SHA spec'ed padding                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	pos = SHA_BUFF_POS(handlePtr->length0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Init SHA digest                                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_SetShaDigest_l(handlePtr->hv, handlePtr->shaType);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Load data into secrun buffer                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_SetBlock_l((UINT8*)handlePtr->block, pos, 0, localBuffer);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set data last byte as in SHA algorithm spec                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_SetBlock_l(&lastbyte, 1, pos++, localBuffer);

	/*-----------------------------------------------------------------------------------------------------*/
	/* If the remainder of data is longer then one block                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	if (pos > (SHA_BLOCK_LENGTH - 8))
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* The message length will be in the next block                                                    */
        /* Pad the rest of the last block with 0's                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        SHA_ClearBlock_l((SHA_BLOCK_LENGTH - pos), pos, localBuffer);

        /*-------------------------------------------------------------------------------------------------*/
        /* Hash the current block                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        SHA_FlushLocalBuffer_l(localBuffer);

        pos = 0;

        /*-------------------------------------------------------------------------------------------------*/
        /* Wait till SHA is not busy                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(SHA_BusyWait_l());
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Pad the rest of the last block with 0's except for the last 8-3 bytes                               */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_ClearBlock_l((SHA_BLOCK_LENGTH-(8-3))-pos, pos, localBuffer);

	/*-----------------------------------------------------------------------------------------------------*/
	/* The last 8-3 bytes are set to the bit-length of the message in big-endian form                      */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_SetLength32_l(handlePtr, localBuffer);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Hash all that, and save the hash for the caller                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_FlushLocalBuffer_l(localBuffer);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait till SHA is not busy                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	DEFS_STATUS_RET_CHECK(SHA_BusyWait_l());

	/*-----------------------------------------------------------------------------------------------------*/
	/* Save SHA final digest into given buffer                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	SHA_GetShaDigest_l(hashDigest, handlePtr->shaType);

	/* SHA_PrintRegs(); */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Free handle                                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	handlePtr->active = false;

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Reset                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reset SHA module                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int SHA_Reset (void)
{
	writeb ( readb(&(sha_dev->HASH_CTR_STS)) | HASH_CTR_STS_SHA_RST, &(sha_dev->HASH_CTR_STS));

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_Power                                                                              */
/*                                                                                                         */
/* Parameters:      on - TRUE enable the module, false disable the module                                  */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set SHA module power on/off                                               */
/*---------------------------------------------------------------------------------------------------------*/
int SHA_Power (bool on)
{
	if ( on == true)
        writeb ( readb(&(sha_dev->HASH_CTR_STS)) | HASH_CTR_STS_SHA_EN, &(sha_dev->HASH_CTR_STS));
	else
        writeb ( readb(&(sha_dev->HASH_CTR_STS)) & ~HASH_CTR_STS_SHA_EN, &(sha_dev->HASH_CTR_STS));

	return 0;
}

#ifdef SHA_PRINT
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_PrintRegs                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void SHA_PrintRegs (void)
{
	UINT i;

	printf("/*--------------*/\n");
	printf("/*     SHA      */\n");
	printf("/*--------------*/\n\n");

	printf("HASH_CTR_STS        = 0x%02X\n", readb(&sha_dev->HASH_CTR_STS));
	printf("HASH_CFG            = 0x%02X\n", readb(&sha_dev->HASH_CFG));

	for (i = 0; i < HASH_DIG_H_NUM; i++)
	{
        printf("HASH_DIG_H%d         = 0x%08X\n", i, readl(&sha_dev->HASH_DIG_H[i]));
	}

	printf("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_PrintVersion                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void SHA_PrintVersion (void)
{
	printf("SHA MODULE VER  = %d\n", readb(&sha_dev->HASH_VER));
}
#endif

#ifdef SHA_SELF_TEST
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_SelfTest                                                                           */
/*                                                                                                         */
/* Parameters:      shaType - SHA module type                                                              */
/* Returns:         0 on success or other int error code on error                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs various tests on the SHA HW and SW                               */
/*---------------------------------------------------------------------------------------------------------*/
int SHA_SelfTest (SHA_TYPE_T shaType)
{
	SHA_HANDLE_T handle;
	UINT8 hashDigest[MAX(SHA_1_HASH_LENGTH, SHA_2_HASH_LENGTH)];
	UINT8 i;
	UINT16 j;

	/*-----------------------------------------------------------------------------------------------------*/
	/* SHA1 tests info                                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	static const UINT8 sha1SelfTestBuff[SHA1_NUM_OF_SELF_TESTS][94] =
	{
        {"abc"},
        {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"},
        {"0123456789012345678901234567890123456789012345678901234567890123"},
        {0x30, 0x5c, 0x30, 0x2c, 0x02, 0x01, 0x00, 0x30, 0x09, 0x06, 0x05, 0x2b,
         0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x30, 0x06, 0x06, 0x04, 0x67, 0x2a,
         0x01, 0x0c, 0x04, 0x14, 0xe1, 0xb6, 0x93, 0xfe, 0x33, 0x43, 0xc1, 0x20,
         0x5d, 0x4b, 0xaa, 0xb8, 0x63, 0xfb, 0xcf, 0x6c, 0x46, 0x1e, 0x88, 0x04,
         0x30, 0x2c, 0x02, 0x01, 0x00, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03,
         0x02, 0x1a, 0x05, 0x00, 0x30, 0x06, 0x06, 0x04, 0x67, 0x2a, 0x01, 0x0c,
         0x04, 0x14, 0x13, 0xc1, 0x0c, 0xfc, 0xc8, 0x92, 0xd7, 0xde, 0x07, 0x1c,
         0x40, 0xde, 0x4f, 0xcd, 0x07, 0x5b, 0x68, 0x20, 0x5a, 0x6c}
	};

	static const UINT8 sha1SelfTestBuffLen[SHA1_NUM_OF_SELF_TESTS] =
	{
        3, 56, 64, 94
	};
	static const UINT8 sha1SelfTestExpRes[SHA1_NUM_OF_SELF_TESTS][SHA_1_HASH_LENGTH] =
	{
        {0xA9, 0x99, 0x3E, 0x36,
         0x47, 0x06, 0x81, 0x6A,
         0xBA, 0x3E, 0x25, 0x71,
         0x78, 0x50, 0xC2, 0x6C,
         0x9C, 0xD0, 0xD8, 0x9D},
        {0x84, 0x98, 0x3E, 0x44,
         0x1C, 0x3B, 0xD2, 0x6E,
         0xBA, 0xAE, 0x4A, 0xA1,
         0xF9, 0x51, 0x29, 0xE5,
         0xE5, 0x46, 0x70, 0xF1},
        {0xCF, 0x08, 0x00, 0xF7,
         0x64, 0x4A, 0xCE, 0x3C,
         0xB4, 0xC3, 0xFA, 0x33,
         0x38, 0x8D, 0x3B, 0xA0,
         0xEA, 0x3C, 0x8B, 0x6E},
        {0xc9, 0x84, 0x45, 0xc8,
         0x64, 0x04, 0xb1, 0xe3,
         0x3c, 0x6b, 0x0a, 0x8c,
         0x8b, 0x80, 0x94, 0xfc,
         0xf3, 0xc9, 0x98, 0xab}
	};

	/*-----------------------------------------------------------------------------------------------------*/
	/* SHA2 tests info                                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	static const UINT8 sha2SelfTestBuff[SHA2_NUM_OF_SELF_TESTS][100] =
	{
        { "abc" },
        { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
        {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
         'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'}
	};

	static const UINT8 sha2SelfTestBuffLen[SHA2_NUM_OF_SELF_TESTS] =
	{
        3, 56, 100
	};

	static const UINT8 sha2SelfTestExpRes[SHA2_NUM_OF_SELF_TESTS][SHA_2_HASH_LENGTH] =
	{
        /*
         * SHA-256 test vectors
         */
        { 0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
          0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
          0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
          0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD },
        { 0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
          0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
          0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
          0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1 },
        { 0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
          0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
          0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
          0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0 }
	};

	/*-----------------------------------------------------------------------------------------------------*/
	/* SHA 1 TESTS                                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	if (shaType == SHA_TYPE_SHA1)
	{
        for(i = 0; i < SHA1_NUM_OF_SELF_TESTS; i++)
        {
            if (i != 3)
            {
                DEFS_STATUS_RET_CHECK(SHA_Calc(SHA_TYPE_SHA1, sha1SelfTestBuff[i], sha1SelfTestBuffLen[i], hashDigest));
            }
            else
            {
                SHA_Power(TRUE);
                SHA_Reset();
                SHA_RET_CHECK(SHA_Start(&handle, SHA_TYPE_SHA1));
                SHA_RET_CHECK(SHA_Update(&handle, sha1SelfTestBuff[i],73));
                SHA_RET_CHECK(SHA_Update(&handle, &(sha1SelfTestBuff[i][73]),sha1SelfTestBuffLen[i] - 73));
                SHA_RET_CHECK(SHA_Finish(&handle, hashDigest));
                SHA_Power(false);
            }
            if (memcmp(hashDigest, sha1SelfTestExpRes[i], SHA_1_HASH_LENGTH))
            {
                return DEFS_STATUS_FAIL;
            }
        }

	}
	/*-----------------------------------------------------------------------------------------------------*/
	/* SHA 2 TESTS                                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	else
	{
        for( i = 0; i < SHA2_NUM_OF_SELF_TESTS; i++ )
        {
            SHA_Power(TRUE);
            SHA_Reset();
            SHA_RET_CHECK(SHA_Start(&handle, SHA_TYPE_SHA2));
            if( i == 2 )
            {
                for( j = 0; j < 10000; j++ )/*not working */
                {
                    SHA_RET_CHECK(SHA_Update(&handle, sha2SelfTestBuff[i], sha2SelfTestBuffLen[i]));
                }
            }
            else
            {
                SHA_RET_CHECK(SHA_Update(&handle, sha2SelfTestBuff[i], sha2SelfTestBuffLen[i]));
            }

            SHA_RET_CHECK(SHA_Finish(&handle, hashDigest));
            SHA_Power(false);
            if(memcmp(hashDigest, sha2SelfTestExpRes[i], SHA_2_HASH_LENGTH))
            {
                return DEFS_STATUS_FAIL;
            }

            SHA_Calc(SHA_TYPE_SHA2, sha2SelfTestBuff[i], sha2SelfTestBuffLen[i], hashDigest);
            if(memcmp(hashDigest, sha2SelfTestExpRes[i], SHA_2_HASH_LENGTH))
            {
                return DEFS_STATUS_FAIL;
            }
        }
	}
	return 0;
}
#endif /*SHA_SELF_TEST */

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL FUNCTIONS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_FlushLocalBuffer_l                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     This routine flush secrun buffer to SHA module                                         */
/*---------------------------------------------------------------------------------------------------------*/
static void SHA_FlushLocalBuffer_l (const UINT32* buff)
{
	UINT i;

	for(i = 0; i < (SHA_BLOCK_LENGTH / sizeof(UINT32)); i++)
	{
        writel ( buff[i] , &sha_dev->HASH_DATA_IN);
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_BusyWait_l                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/* Returns:         0 if no error was found or DEFS_STATUS_ERROR otherwise                    */
/* Side effects:                                                                                           */
/* Description:     This routine wait for SHA unit to no longer be busy                                    */
/*---------------------------------------------------------------------------------------------------------*/
static int SHA_BusyWait_l (void)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* While SHA module is busy                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	UINT32 __time = SHA_TIMEOUT;

	do
	{
        if (__time-- == 0)
        {
            return -(ETIMEDOUT);
        }
	} while ((readb(&(sha_dev->HASH_CTR_STS)) & 0x02) == 0x02);

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_GetShaDigest_l                                                                     */
/*                                                                                                         */
/* Parameters:      hashDigest - buffer for the hash output.                                               */
/*                  shaType - SHA module type                                                              */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     This routine copy the hash digest from the hardware and into given buffer( in ram)     */
/*---------------------------------------------------------------------------------------------------------*/
static void SHA_GetShaDigest_l(
	UINT8* hashDigest,
	SHA_TYPE_T shaType
)
{
	UINT16 j;
	UINT32* src =     (UINT32 *)(&sha_dev->HASH_DIG_H[0]);
	UINT32* dest =    (UINT32 *)(void*)hashDigest;
	UINT8 len = ((shaType == SHA_TYPE_SHA2) ? SHA_2_HASH_LENGTH : SHA_1_HASH_LENGTH) / sizeof(UINT32);

	/*-------------------------------------------------------------------------------------------------------*/
	/* Copy Bytes from SHA module to given buffer                                                            */
	/*-------------------------------------------------------------------------------------------------------*/
	for (j = 0; j < len; j++)
	{
	writel(src[j], &dest[j]);
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_SetShaDigest_l                                                                     */
/*                                                                                                         */
/* Parameters:      hashDigest - input buffer to set as hash digest(in SHA module).                        */
/*                  shaType - SHA module type                                                              */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     This routine set the hash digest in the hardware from a given buffer( in ram)          */
/*---------------------------------------------------------------------------------------------------------*/
static  void SHA_SetShaDigest_l(
	const UINT32* hashDigest,
	SHA_TYPE_T shaType
)
{
	UINT16 j;
	volatile UINT32* dest = &(sha_dev->HASH_DIG_H[0]);
	UINT8 len = ((shaType == SHA_TYPE_SHA2) ? SHA_2_HASH_LENGTH : SHA_1_HASH_LENGTH) / sizeof(UINT32);

	/*-------------------------------------------------------------------------------------------------------*/
	/* Copy Bytes from given buffer to SHA module                                                            */
	/*-------------------------------------------------------------------------------------------------------*/
	for (j = 0; j < len; j++)
	{
	writel( hashDigest[j], &dest[j]);
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_SetBlock_l                                                                         */
/*                                                                                                         */
/* Parameters:      data        - data to copy                                                             */
/*                  len         -  size of data                                                            */
/*                  position    - byte offset into the block at which data should be placed                */
/*                  block       - block buffer                                                             */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     This routine load bytes into block buffer                                              */
/*---------------------------------------------------------------------------------------------------------*/
static void SHA_SetBlock_l(const UINT8* data,UINT32 len, UINT16 position, UINT32* block)
{
	UINT8 * dest = (UINT8*)block;
	memcpy(dest + position, data, len);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_SetBlock_l                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  len -  size of data                                                                    */
/*                  position - byte offset into the block at which data should be placed                   */
/*                  block - block buffer                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     This routine load zero's into the block buffer                                         */
/*---------------------------------------------------------------------------------------------------------*/
static void SHA_ClearBlock_l (
	UINT16 len,
	UINT16 position,
	UINT32* block
)
{
	UINT8 * dest = (UINT8*)block;
	memset(dest + position, 0, len);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SHA_SetLength32_l                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  handlePtr  -   SHA processing handle pointer                                           */
/*                  block - block buffer                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:     This routine set the length of the hash's data                                         */
/*                  len is the 32-bit byte length of the message                                           */
/*lint -efunc(734,SHA_SetLength32_l) Supperess loss of percision lint warning                              */
/*---------------------------------------------------------------------------------------------------------*/
static void SHA_SetLength32_l (const SHA_HANDLE_T* handlePtr, UINT32* block)
{
	UINT16*       secrunBufferSwappedPtr = (UINT16*)(void*)(block);

	secrunBufferSwappedPtr[(SHA_BLOCK_LENGTH/sizeof(UINT16)) - 1] = (UINT16)
        ((handlePtr->length0 << 3) << 8) | ((UINT16) (handlePtr->length0 << 3) >> 8);
	secrunBufferSwappedPtr[(SHA_BLOCK_LENGTH/sizeof(UINT16)) - 2] = (UINT16)
        ((handlePtr->length0 >> (16-3)) >> 8) | ((UINT16) (handlePtr->length0 >> (16-3)) << 8);
	secrunBufferSwappedPtr[(SHA_BLOCK_LENGTH/sizeof(UINT16)) - 3] = (UINT16)
	((handlePtr->length1 << 3) << 8) | ((UINT16) (handlePtr->length1 << 3) >> 8);
	secrunBufferSwappedPtr[(SHA_BLOCK_LENGTH/sizeof(UINT16)) - 4] = (UINT16)
	((handlePtr->length1 >> (16-3)) >> 8) | ((UINT16) (handlePtr->length1 >> (16-3)) << 8);
}

#endif /* CONFIG_SHA_HW_ACCEL */




