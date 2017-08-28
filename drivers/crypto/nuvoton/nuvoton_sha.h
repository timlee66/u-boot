/*---------------------------------------------------------------------------------------------------------*/
/*	Nuvoton Technology Corporation Confidential 														   */
/*																										   */
/*	Copyright (c)      2016 by Nuvoton Technology Corporation											   */
/*	All rights reserved 																				   */
/*																										   */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:																						   */
/*	 nuvoton_sha.h																						   */
/*			  This file contains HW IF to nuvoton BMC SHA engine            							   */
/*---------------------------------------------------------------------------------------------------------*/



#ifndef __NUVOTON_NPCM750_SHA_H
#define __NUVOTON_NPCM750_SHA_H


#define SHA_PHYS_BASE_ADDR       0xF085A000
#define SHA_BASE_ADDR            SHA_PHYS_BASE_ADDR

#if 1
#define padding(name,size)    __pad_##name##_[size]
#pragma pack(1)
typedef struct nuvoton_bmc_sha {
	u32 HASH_DATA_IN;
	u8  HASH_CTR_STS;       u8 padding(HASH_CTR_STS,3);
	u8  HASH_CFG;           u8 padding(HASH_CFG,3);
	u8  HASH_VER;           u8 padding(HASH_VER,3);
	u32 reserved[4];
	u32 HASH_DIG_H[8];
} nuvoton_bmc_sha;
#pragma pack()
#undef padding


#else
#define HASH_DATA_IN            (SHA_BASE_ADDR + 0x000)
#define HASH_CTR_STS            (SHA_BASE_ADDR + 0x004)
#define HASH_CFG                (SHA_BASE_ADDR + 0x008)
#define HASH_VER                (SHA_BASE_ADDR + 0x00C)
#define HASH_DIG_H(i)           (SHA_BASE_ADDR + 0x020 + (4 * i))
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* HASH_CTR_STS fields                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define HASH_CTR_STS_SHA_EN     (1 << 0)
#define HASH_CTR_STS_SHA_BUSY   (1 << 1)
#define HASH_CTR_STS_SHA_RST    (1 << 2)

/*---------------------------------------------------------------------------------------------------------*/
/* HASH_CFG fields                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define HASH_CFG_SHA1_SHA2      (1 << 0)

/*---------------------------------------------------------------------------------------------------------*/
/* HASH_DIG_H fields                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define HASH_DIG_H_NUM          8

#define SHA_MODULE_TYPE         1




/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TYPES & DEFINITIONS                                    */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

#define SHA_BLOCK_LENGTH     (512/8)
#define SHA_2_HASH_LENGTH    (256/8)
#define SHA_1_HASH_LENGTH    (160/8)

/*---------------------------------------------------------------------------------------------------------*/
/* SHA type                                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	SHA_TYPE_SHA2 = 0,/*do not change - match SHA arch spec */
	SHA_TYPE_SHA1,
	SHA_TYPE_NUM
} SHA_TYPE_T;

/*---------------------------------------------------------------------------------------------------------*/
/* SHA instance struct handler                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct SHA_HANDLE_T
{
	UINT32              hv[SHA_2_HASH_LENGTH / sizeof(UINT32)];
	UINT32              length0;
	UINT32              length1;
	UINT32              block[SHA_BLOCK_LENGTH / sizeof(UINT32)];
	SHA_TYPE_T          shaType;
	BOOLEAN             active;
} SHA_HANDLE_T;




#endif /* __NUVOTON_NPCM750_SHA_H */
