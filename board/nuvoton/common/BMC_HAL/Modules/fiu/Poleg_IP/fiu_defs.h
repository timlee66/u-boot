/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   fiu_defs.h                                                                                            */
/*            This file contains FIU definitions                                                           */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _FIU_DEFS_H
#define _FIU_DEFS_H


/*---------------------------------------------------------------------------------------------------------*/
/* Defines                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

#define FIU_CAPABILITY_QUAD_READ
#define FIU_CAPABILITY_SUPPORT_128MB_FLASH
#define FIU_CAPABILITY_CHIP_SELECT
#define FIU_CAPABILITY_SPI_X

#define WIN_LIMIT_4K_SHIFT  12
#define BITS_7_0            0xFF
#define BITS_15_8           0xFF00
#define BITS_23_16          0xFF0000


/*---------------------------------------------------------------------------------------------------------*/
/* Typedef Definitions                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum _spi_w_burst_t
{
	FIU_W_BURST_ONE_BYTE        = 0,
/*    FIU_W_BURST_FOUR_BYTE       = 2, */
	FIU_W_BURST_SIXTEEN_BYTE    = 3
} SPI_w_burst_t;

typedef enum _spi_r_burst_t
{
	FIU_R_BURST_ONE_BYTE        = 0,
/*    FIU_R_BURST_FOUR_BYTE       = 2, */
	FIU_R_BURST_SIXTEEN_BYTE    = 3
} SPI_r_burst_t;

typedef enum _spi_w_protect_int_t
{
	SPI_W_PROTECT_INT_DISABLE = 0,
	SPI_W_PROTECT_INT_ENABLE  = 1
} SPI_w_protect_int_t;

typedef enum _spi_incorect_access_int_t
{
	SPI_INCORECT_ACCESS_INT_DISABLE   = 0,
	SPI_INCORECT_ACCESS_INT_ENABLE    = 1
} SPI_incorect_access_int_t;

/*---------------------------------------------------------------------------------------------------------*/
/* FIU Read Mode                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum _spi_read_mode_t
{
	FIU_NORMAL_READ             = 0,
	FIU_FAST_READ               = 1,
	FIU_FAST_READ_DUAL_OUTPUT   = 2,
	FIU_FAST_READ_DUAL_IO       = 3,
#ifdef FIU_CAPABILITY_QUAD_READ
	FIU_FAST_READ_QUAD_IO       = 4,
#endif
#ifdef FIU_CAPABILITY_SPI_X
	FIU_FAST_READ_SPI_X         = 5,
#endif
	FIU_READ_MODE_NUM

} SPI_read_mode_t;

/*---------------------------------------------------------------------------------------------------------*/
/* FIU Device Size                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum _spi_dev_size_t
{
	FIU_DEV_SIZE_128KB          = 1,
	FIU_DEV_SIZE_256KB          = 2,
	FIU_DEV_SIZE_512KB          = 4,
	FIU_DEV_SIZE_1MB            = 8,
	FIU_DEV_SIZE_2MB            = 16,
	FIU_DEV_SIZE_4MB            = 32,
#if defined FIU_CAPABILITY_SUPPORT_128MB_FLASH || defined FIU_CAPABILITY_SUPPORT_64MB_FLASH || defined FIU_CAPABILITY_SUPPORT_16MB_FLASH || defined FIU_CAPABILITY_SUPPORT_8MB_FLASH
	FIU_DEV_SIZE_8MB            = 64,
#if defined FIU_CAPABILITY_SUPPORT_128MB_FLASH || defined FIU_CAPABILITY_SUPPORT_64MB_FLASH || defined FIU_CAPABILITY_SUPPORT_16MB_FLASH
	FIU_DEV_SIZE_16MB           = 128,
#if defined FIU_CAPABILITY_SUPPORT_128MB_FLASH || defined FIU_CAPABILITY_SUPPORT_64MB_FLASH
	FIU_DEV_SIZE_32MB           = 256,
	FIU_DEV_SIZE_64MB           = 512,
#if defined FIU_CAPABILITY_SUPPORT_128MB_FLASH
	FIU_DEV_SIZE_128MB          = 1024
#endif
#endif
#endif
#endif
} SPI_dev_size_t;





/*---------------------------------------------------------------------------------------------------------*/
/* FIU UMA data size                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum _spi_uma_data_size_t
{
	FIU_UMA_DATA_SIZE_0         = 0,
	FIU_UMA_DATA_SIZE_1         = 1,
	FIU_UMA_DATA_SIZE_2         = 2,
	FIU_UMA_DATA_SIZE_3         = 3,
	FIU_UMA_DATA_SIZE_4         = 4,
	FIU_UMA_DATA_SIZE_5         = 5,
	FIU_UMA_DATA_SIZE_6         = 6,
	FIU_UMA_DATA_SIZE_7         = 7,
	FIU_UMA_DATA_SIZE_8         = 8,
	FIU_UMA_DATA_SIZE_9         = 9,
	FIU_UMA_DATA_SIZE_10         = 10,
	FIU_UMA_DATA_SIZE_11         = 11,
	FIU_UMA_DATA_SIZE_12         = 12,
	FIU_UMA_DATA_SIZE_13         = 13,
	FIU_UMA_DATA_SIZE_14         = 14,
	FIU_UMA_DATA_SIZE_15         = 15,
	FIU_UMA_DATA_SIZE_16         = 16

} SPI_uma_data_size_t;


/*---------------------------------------------------------------------------------------------------------*/
/* FIU Field value enumeration                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum _spi_drd_cfg_addsiz_t
{
	FIU_DRD_CFG_ADDSIZE_24BIT = 0,   /* 0 0: 24 bits (3 bytes) (default).  */
	FIU_DRD_CFG_ADDSIZE_32BIT = 1    /* 0 1: 32 bits (4 bytes)             */
}  SPI_drd_cfg_addsiz_t;


typedef enum _spi_trans_status_t
{
	FIU_TRANS_STATUS_DONE        = 0,
	FIU_TRANS_STATUS_IN_PROG     = 1
} SPI_trans_status_t;


#if 0
/*---------------------------------------------------------------------------------------------------------*/
/* FIU Chip Select                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum  FIU_CS_T_tag
{
	FIU_CS_0 = 0,
	FIU_CS_1 = 1,
	FIU_CS_2 = 2,
	FIU_CS_3 = 3
} FIU_CS_T;

/*---------------------------------------------------------------------------------------------------------*/
/* FIU Illegal Address (IAD) trap generation                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum FIU_IAD_TRAP_T_tag
{
	FIU_IAD_TRAP_DISABLE          = 0,
	FIU_IAD_TRAP_ENABLE           = 1
} FIU_IAD_TRAP_T;

#endif



#endif /*_FIU_DEFS_H */
