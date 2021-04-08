/*--------------------------------------------------------------------------
 * Copyright (c) 2009-2010 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------*/
#ifndef _CORE_GENERAL_H_
#define _CORE_GENERAL_H_

/*------------------------------------------------------------------------*/
/*----------------   Constants and macros definitions  -------------------*/
/*------------------------------------------------------------------------*/

#define END_OF_TABLE				0xFFFF

/* Used by PrintEnum() function. See function header */

// To print the enum string
enum
{
	ENUM_RD_MODE		// This is only an example!
};

// This ENUM is an example (used by enumStringTable):
enum
{
	RD_MODE_NORMAL = 0,
	RD_MODE_FAST,
	RD_MODE_FAST_DUAL,
	RD_MODE_RESERVED
};

enum
{
	ENUM_ID_DEVICE		// Identifier of DEV_ID
};

// ENUM for CHIP_ID
enum
{
	ID_CHIP_09A = 0x10,
	ID_CHIP_09B = 0x40,
	ID_CHIP_10A = 0x50,
	ID_CHIP_10B = 0x55,
	ID_CHIP_11A = 0x04,
	ID_CHIP_12A = 0x05
};

// ENUM for DEV_ID
enum
{
	ID_11A_DEVICE_985W = 0x02,
	ID_11A_DEVICE_985L = 0x07,
	ID_11A_DEVICE_985C = 0x00,
	ID_11A_DEVICE_985E = 0x01,
	ID_11A_DEVICE_985U = 0x08,
	ID_11A_DEVICE_995L = 0x0C,
	ID_11A_DEVICE_995U = 0x0D,
	ID_11A_DEVICE_985P = 0x0B,
	ID_11A_DEVICE_985G = 0x09,
	ID_11A_DEVICE_988L = 0x04,

	ID_12A_DEVICE_285W = 0x02,
	ID_12A_DEVICE_285L = 0x07,
	ID_12A_DEVICE_285U = 0x08,
	ID_12A_DEVICE_295L = 0x0C,
	ID_12A_DEVICE_295U = 0x0D,
	ID_12A_DEVICE_285S = 0x01,
	ID_12A_DEVICE_285P = 0x0B,
	ID_12A_DEVICE_285G = 0x09,
	ID_12A_DEVICE_288L = 0x06,
	ID_12A_DEVICE_241L = 0x10,
	ID_12A_DEVICE_248L = 0x13,
	ID_12A_DEVICE_248U = 0x1B
};

/*------------------------------------------------------------------------*/
/*---------------------   Data Types definitions   -----------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*---------------------------   Externals   ------------------------------*/
/*------------------------------------------------------------------------*/

extern	enumString*	enumStringTablePtr;
extern	BOOL CorePrintFlag;
/*------------------------------------------------------------------------*/
/*-----------------------   Function headers   ---------------------------*/
/*------------------------------------------------------------------------*/

DWORD	Get_Random_Value								(DWORD low_value, DWORD high_value);

/*--------------------------------------------------------------------------*/
/* Function:	IRQ_TestHandler												*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:Call from an Interrupt handler								*/
/*																			*/
/* Description:                                                             */
/*		This function is called from an interrupt handler routine.			*/
/*		A code should be added by the tester in order to check the source of*/
/*		the interrupt and also update test parameters according to the test	*/
/*--------------------------------------------------------------------------*/
//void	IRQ_TestHandler					(void);

#endif // _CORE_GENERAL_H_
