/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
/* Project: Yarkon Security Library                                                                        */
/* File Name: AES_if.h                                                                                     */
/*                                                                                                         */
/* File Contents:                                                                                          */
/*      This file contains Advanced Encryption Standard (AES) algorithm                                    */
/*      driver definitions.                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef AES_IF_H
#define AES_IF_H

#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/* AES module enumerations                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/* AES Operation */
typedef enum
{
	AES_ENCRYPT,
	AES_DECRYPT
} AES_OP_T;

/* AES Mode */
typedef enum
{
	AES_MODE_ECB,   /* Electronic Codebook  */
	AES_MODE_CBC,   /* Cipher Block Chaining */
	AES_MODE_CTR,   /* Counter */
	AES_MODE_MAC    /* Message Authentication Code */
} AES_MODE_T;

/* AES Key Size */
typedef enum
{
	AES_128,
	AES_192,
	AES_256
} AES_KEY_SIZE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* AES module macro definitions                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
/* The bit length of supported keys for the AES algorithm */
#define AES_KEY_BIT_SIZE(size)  (128 + (AES_KEY_SIZE_T)(size) * 64)

/* The byte length of supported keys for the AES algorithm */
#define AES_KEY_BYTE_SIZE(size) (AES_KEY_BIT_SIZE(size) / 8)

/* # of bytes needed to represent a key */
#define AES_MAX_KEY_SIZE        AES_KEY_BYTE_SIZE(AES_256)

/* The byte length of a block for the AES algorithm (b = 128 bit) */
#define AES_BLOCK_SIZE          AES_KEY_BYTE_SIZE(AES_128)

/* # of bytes needed to represent an IV  */
#define AES_MAX_IV_SIZE         AES_BLOCK_SIZE

/* # of bytes needed to represent a counter  */
#define AES_MAX_CTR_SIZE        AES_BLOCK_SIZE

/* Calculate the number of blocks in the formatted message */
#define AES_COMPLETE_BLOCKS(size)   ((size + (AES_BLOCK_SIZE - 1)) / AES_BLOCK_SIZE)

/*---------------------------------------------------------------------------------------------------------*/
/* AES module structures                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*  The structure for cipher information */
typedef struct {
	AES_OP_T    operation;  /* Encrypt / Decrypt */
	AES_MODE_T  mode;       /* ECB / CBC / CTR / MAC */
	const UINT8 *IV;         /* (optional) Initialization Vector for ciphering */
	UINT8       *CTR;        /* (optional) Counter for ciphering */
} AES_CIPHER_INSTANCE;

/*  The structure for key information */
typedef struct {
	AES_KEY_SIZE_T  keyLen; /* Bit length of the key - 128 / 192 / 256  */
	UINT8           index;  /* Key address in the fuse array in 128-bit steps (for loading a key from OTP) */
	UINT8          *key;    /* Raw key data */
} AES_KEY_INSTANCE;

/*  The structure for data information */
typedef struct {
	UINT32       size;      /* Byte length of input data */
	const UINT8 *input;     /* Input data */
	UINT8       *output;    /* Output data */
} AES_DATA_INSTANCE;



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AES_Crypt                                                                              */
/*                                                                                                         */
/* Parameters:      cipher - The structure for cipher information [input].                                 */
/*                  key    - The structure for key information    [input].                                 */
/*                  data   - The structure for data information   [input].                                 */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*      Encrypt/Decrypt a message AES                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void AES_Crypt (AES_CIPHER_INSTANCE *cipher, AES_KEY_INSTANCE *key, AES_DATA_INSTANCE *data);

#endif /*AES_IF_H */

