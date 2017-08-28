/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hal_error.h                                                                                           */
/*            This file contains BMC HAL error codes                                                       */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _HAL_ERROR_H
#define _HAL_ERROR_H


/*---------------------------------------------------------------------------------------------------------*/
/* BMC HAL Error codes list                                                                                */
/*---------------------------------------------------------------------------------------------------------*/

typedef enum
{
	HAL_OK                          = 0x00,
	HAL_ERROR                       = 0x01,
	HAL_ERROR_BAD_FREQ              = 0x02,
	HAL_ERROR_BAD_DEVNUM            = 0x03,
	HAL_ERROR_DMA_NOT_OWNED         = 0x04,
	HAL_ERROR_PHY                   = 0x05,
	HAL_ERROR_LOCKED                = 0x06,
	HAL_ERROR_ILLEGAL_SIZE          = 0x07,
	HAL_ERROR_NOT_INITIALIZED       = 0x08,
	HAL_ERROR_BAD_ADDRESS           = 0x09,
	HAL_ERROR_BAD_PARAM             = 0x0A,
	HAL_ERROR_TIMEOUT               = 0x0B,
	HAL_ERROR_NOT_HANDLED           = 0x0C,
	HAL_ERROR_BAD_PARITY            = 0x0D,
	HAL_ERROR_OVERRUN_OCCURRED      = 0x0E,
	HAL_ERROR_BREAK_OCCURRED        = 0x0F,
	HAL_ERROR_BAD_FRAME             = 0x10,
	HAL_ERROR_QUEUE_EMPTY           = 0x11,
	HAL_ERROR_QUEUE_FULL            = 0x12,
	HAL_ERROR_NO_LINK               = 0x13,
	HAL_ERROR_ILLEGAL_CLK           = 0x14,
	HAL_ERROR_PACKET_ABORT          = 0x15,
	HAL_ERROR_CARRIER               = 0x16,
	HAL_ERROR_BIG_PACKET            = 0x17,
	HAL_ERROR_CRC                   = 0x18,
	HAL_ERROR_SMALL_PACKET          = 0x19,
	HAL_ERROR_FIFO_OVERFLOW         = 0x1A,
	HAL_ERROR_BUS                   = 0x1B,
	HAL_ERROR_BAD_ALIGNMENT         = 0x1C,
	HAL_ERROR_NO_MEDIA              = 0x1D,
	HAL_ERROR_WRITE_PROTECT         = 0x1E,
	HAL_ERROR_ILLEGAL_VALUE         = 0x1F,
	HAL_ERROR_BAD_SIZE              = 0x20,
	HAL_ERROR_NOT_IMPLEMENTED       = 0x21
} HAL_STATUS;


#endif /*_HAL_ERROR_H */

