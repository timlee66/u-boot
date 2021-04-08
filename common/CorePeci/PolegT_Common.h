/*-------------------------------------------------------------------------
 * Copyright (c) 2008-2010 by Nuvoton Technology Israel
 * All rights reserved.
 *
 * Contents:	Includes all definitions which are for external use by both
 *				Host and Core.
 *------------------------------------------------------------------------*/
#ifndef __POLEG_T_COMMON_H__
#define __POLEG_T_COMMON_H__

#include <tl_common.h>
#include <CoreRegisters.h>

/*------------------------------------------------------------------------*/
/*----------------   Constants and macros definitions  -------------------*/
/*------------------------------------------------------------------------*/

#define	TEMPLATE_VERSION				"2.12D"

/*------------------*/
/* Message printing */
/*------------------*/
#define FUNC_PRINT(str)					\
	if (FuncPrintToLog)					\
	{									\
		LogMessage str;					\
	}

/*------*/
/* Time */
/*------*/
#define HALF_SECOND						(ONE_SECOND / 2)
#define ONE_SECOND						1000				// In milliseconds
#define ONE_MINUTE						(60 * ONE_SECOND)
#define ONE_HOUR						(60 * ONE_MINUTE)

/*------------*/
/* Conversion */
/*------------*/
#define SWAP16(u16) \
            ( ((WORD)((u16) & 0x00FF) << 8)         \
            | (((WORD)(u16) & 0xFF00) >> 8))				/* Swap high byte and low byte               */

#define SWAP32(u32) \
            ( ((DWORD)((u32) & 0x0000FFFF) << 16)    \
            | (((DWORD)(u32) & 0xFFFF0000) >> 16))			/* Swap high word and low word               */

// Write a variable's specific single bit with 1 or 0
// Note: Decided not to use SET_BIT(var, bitNum) and CLEAR_BIT(var, bitNum), for two reasons:
//       1) It does not match the SET_FIELD macro, which also excepts two parameters
//       2) In some cases using SET and CLEAR, requires adding and 'if' case.
//          For example: "if (enable) SET_BIT(x, y) else CLEAR_BIT(x, y)"
//          Instead use: "SET_BIT(x, y, z)" without a 'if' case.
#define	SET_BIT(var, bitNum, data)	(SET_FIELD(var, bitNum, bitNum, data))

// Check if bits are set or cleared
#define BIT_IS_SET(data, bitNum)	((data >> bitNum) & 1)
#define BIT_IS_CLEARED(data, bitNum) (!((data >> bitNum) & 1))

// Creates a mask from a single bit
//#define BIT_MASK(bit)					(1 << bit)

// Creates a mask of a range of bits (field)
#define FIELD_MASK(firstBit, lastBit)	((((1 << (lastBit - firstBit + 1)) - 1)) << firstBit)

// Write a variable's specific bits (field) with a value (if the data value is greater than the max value the field can hold, the data is truncated)
#define SET_FIELD(var, firstBit, lastBit, data)	(var = (var & ~(FIELD_MASK(firstBit, lastBit)) | ((data & FIELD_MASK(firstBit, lastBit)) << firstBit)))

// Reads a variable's specific bits (field)
#define GET_FIELD(var, firstBit, lastBit)	((var & FIELD_MASK(firstBit, lastBit)) >> firstBit)

// Read a variable's specific bit
#define GET_BIT(var, bitNum)		(GET_FIELD(var, bitNum, bitNum))

// Write a register's specific bits (field) with a value
#define SET_FIELD_REG(reg, firstBit, lastBit, data)	(WRITE_REG(reg, (READ_REG(reg) & (~(FIELD_MASK(firstBit, lastBit)))) | (data << firstBit)))

// Read specific bits (field) from a register
#define GET_FIELD_REG(reg, firstBit, lastBit)	(GET_FIELD(READ_REG(reg), firstBit, lastBit))

/*------------*/
/* Arithmetic */
/*------------*/
#define ROUND_DOWN(val, n)				((val / n) * n)		/* Round (down) the number (val) on the (n) boundary */

#define MAX(a, b)						( (a)>(b) ? (a) : (b) )		// Calculate maximum value
#define MIN(a, b)						( (a)<(b) ? (a) : (b) )		// Calculate minimum value

/*-------------*/
/* Test macros */
/*-------------*/

#define TEST_FAILED				0						// Return value of tests and auxiliary functions
#define TEST_PASSED				1

#define	FUNC_PASSED				0
#define	FUNC_FAILED				1

/*------------------------------------------------------------------------*/
/*---------------------   Data Types definitions   -----------------------*/
/*------------------------------------------------------------------------*/

typedef enum Reset_Type
{
	RESET_VSB = 0,
	RESET_VDD,
	RESET_PCI,
	RESET_CORE,
	RESET_WATCHDOG
} Reset_Type;

/*------------------------------------------------------------------------*/
/*---------------------------   Externs   --------------------------------*/
/*------------------------------------------------------------------------*/
extern	BOOL	FuncPrintToLog;

/*------------------------------------------------------------------------*/
/*-----------------------   Function headers   ---------------------------*/
/*------------------------------------------------------------------------*/

#endif	// __POLEG_T_COMMON_H__
