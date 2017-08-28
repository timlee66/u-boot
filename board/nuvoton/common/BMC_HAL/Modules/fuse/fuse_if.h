/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   fuse_if.h                                                                                            */
/*            This file contains API of FUSE module routines for ROM code                                  */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef FUSE_IF_H
#define FUSE_IF_H

#include "defs.h"
#include "../aes/aes_if.h"
#include "../../Common/hal_error.h"



/*---------------------------------------------------------------------------------------------------------*/
/* Fuse module definitions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define FUSE_ARR_BYTE_SIZE  128
#define KEYS_ARR_BYTE_SIZE  128

#define RSA_KEY_BYTE_SIZE       256

/*---------------------------------------------------------------------------------------------------------*/
/* Fuse ECC type                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum FUSE_ECC_TYPE_tag
{
	FUSE_ECC_MAJORITY = 0,
	FUSE_ECC_NIBBLE_PARITY = 1,
	FUSE_ECC_NONE = 2
}  FUSE_ECC_TYPE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Fuse key Type                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum FUSE_KEY_TYPE_tag
{
	FUSE_KEY_AES = 0,
	FUSE_KEY_RSA = 1
}  FUSE_KEY_TYPE_T;



/*---------------------------------------------------------------------------------------------------------*/
/* Fuse module enumerations                                                                                */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Storage Array Type:                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (NPCM750)
typedef enum
{
	KEY_SA    = 0,
	FUSE_SA   = 1
} FUSE_STORAGE_ARRAY_T;
#else
typedef enum
{
	KEY_SA,
	FUSE_SA
} STORAGE_ARRAY_T;
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FUSTRAP fields definition                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum FUSE_FUSTRAP_FIELDS_T_tag
{
	FUSE_FUSTRAP_DIS_FAST_BOOT = 29,                    /* (Disable Fast Boot). */
	FUSE_FUSTRAP_Bit_28 = 28,                            /* unknown register field ! */
	FUSE_FUSTRAP_oWDEN = 27,                            /* (Watchdog Enable). */
	FUSE_FUSTRAP_oHLTOF = 26,                           /* (Halt on Failure). I */
	FUSE_FUSTRAP_oAESKEYACCLK = 25,                     /* (AES Key Access Lock). */
	FUSE_FUSTRAP_oJDIS = 24,                            /* (JTAG Disable). */
	FUSE_FUSTRAP_oSECBOOT = 23,                         /* (Secure Boot). */
	FUSE_FUSTRAP_USEFUSTRAP = 22,                       /* */
	FUSE_FUSTRAP_oPKInvalid2_0 = 19,                    /* */
	FUSE_FUSTRAP_oAltImgLoc = 18,                       /*  Alternate image location definition. */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Added on Z2                                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	FUSE_FUSTRAP_oHINDDIS         =      14,            /* oHINDDIS: disable eSPI independent mode */
	FUSE_FUSTRAP_oSecBootDisable  =      15,            /* {oSecBootDisable} - when set, disables capability enter Secure Mode. Used for Derivatives. */
	FUSE_FUSTRAP_oCPU1STOP2 =            16,            /* {oCPU1STOP2} - when set, stops CPU core 1 clock. */
	FUSE_FUSTRAP_oCPU1STOP1 =            17             /* {oCPU1STOP1} - when set, CPU core 1 stops and cannot be woken. */


} FUSE_FUSTRAP_FIELDS_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Fuse module exported functions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* HW level functions:                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (NPCM750)
HAL_STATUS   FUSE_Init                      (void);
void         FUSE_Read                      (FUSE_STORAGE_ARRAY_T arr, UINT16 addr, UINT8 *data);
HAL_STATUS   FUSE_ProgramBit                (FUSE_STORAGE_ARRAY_T arr, UINT16 byteNum, UINT8 bitNum);
HAL_STATUS   FUSE_ProgramByte               (FUSE_STORAGE_ARRAY_T arr, UINT16 byteNum, UINT8 value);
BOOLEAN      FUSE_BitIsProgrammed           (FUSE_STORAGE_ARRAY_T arr, UINT16 byteNum, UINT8 bitNum);
void         FUSE_UploadKey                 (AES_KEY_SIZE_T keySize, UINT8 keyIndex);
HAL_STATUS   FUSE_ReadKey                   (FUSE_KEY_TYPE_T  keyType, UINT8  keyIndex, UINT8 *output);
void         FUSE_DisableKeyAccess          (void);
HAL_STATUS   FUSE_SelectKey                 (UINT8 keyIndex);
HAL_STATUS   FUSE_LockAccess                (FUSE_STORAGE_ARRAY_T array, UINT8 lockForRead, UINT8 lockForWrite, BOOLEAN lockRegister);

/*---------------------------------------------------------------------------------------------------------*/
/* ECC handling                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS   FUSE_NibParEccDecode           (UINT8 *datain,    UINT8 *dataout,    UINT32  size);
HAL_STATUS   FUSE_NibParEccEncode           (UINT8 *datain,    UINT8 *dataout,    UINT32  size);
HAL_STATUS   FUSE_MajRulEccDecode           (UINT8 *datain,    UINT8 *dataout,    UINT32  encoded_size);
HAL_STATUS   FUSE_MajRulEccEncode           (UINT8 *datain,    UINT8 *dataout,    UINT32  encoded_size);

/*---------------------------------------------------------------------------------------------------------*/
/* FUSTRAP register getter                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
UINT          FUSE_Fustrap_Get               (FUSE_FUSTRAP_FIELDS_T oFuse);

#else

void        FUSE_Init                      (UINT8 APBclock);
void        FUSE_Read                      (STORAGE_ARRAY_T arr, UINT8 addr, UINT8 *data);
HAL_STATUS  FUSE_ProgramBit                (STORAGE_ARRAY_T arr, UINT8 byteNum, UINT8 bitNum);
HAL_STATUS  FUSE_ProgramByte               (STORAGE_ARRAY_T arr, UINT8 byteNum, UINT8 value);
BOOLEAN     FUSE_BitIsProgrammed           (STORAGE_ARRAY_T arr, UINT8 byteNum, UINT8 bitNum);
void        FUSE_UploadKey                 (AES_KEY_SIZE_T keySize, UINT8 keyIndex);
HAL_STATUS  FUSE_ReadKey                   (AES_KEY_SIZE_T keySize, UINT8 keyIndex, UINT8 *output);
void        FUSE_DisableKeyAccess          (void);

/*Logical level functions: */
void        FUSE_NibParEccDecode           (UINT8 *datain, UINT8* dataout, UINT8 size);
HAL_STATUS  FUSE_MajRulEccDecode           (UINT8 *datain, UINT8* dataout, UINT8 size);
void        FUSE_ReadFuseProtectionWord    (UINT16 *word);
void        FUSE_ReadDerivatives           (UINT32 *dword);
void        FUSE_ReadVerFaultProt          (UINT32 *dword);
#endif

#endif /* FUSE_IF_H */
