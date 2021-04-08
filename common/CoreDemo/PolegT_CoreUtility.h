/*--------------------------------------------------------------------------
 * Copyright (c) 2008-2009 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------
 * File Contents:
 *           Global utility functions.
 * Project : Poleg
 * Author  : Shahar Ben-Varon
 *--------------------------------------------------------------------------*/
#ifndef _T_UTILITY_H_
#define _T_UTILITY_H_

#include "tl_common.h"
#include "PolegT_CoreCommon.h"

/*------------------------------------------------------------------------*/
/*----------------   Constants and macros definitions  -------------------*/
/*------------------------------------------------------------------------*/

#define END_OF_TABLE				0xFFFF

#define NUM_OF_TIMERS				4
#define	TIMER_MAX_VALUE				0x00FFFFFF					// Maximum value which the timer can start countdown
#define HALF_SECOND					(ONE_SECOND / 2)
#define ONE_SECOND					1000						// In milliseconds
#define ONE_MINUTE					(60 * ONE_SECOND)
#define ONE_HOUR					(60 * ONE_MINUTE)

#define MAX(a, b)					( (a)>(b) ? (a) : (b) )		// Calculate maximum value
#define MIN(a, b)					( (a)<(b) ? (a) : (b) )		// Calculate minimum value

#define ROUND_DOWN(val, n)			((val / n) * n)			/* Round (down) the number (val) on the (n) boundary */

#define	INT_TO_STR_LEN	15

#define UPDATE_TEST_STATUS(returnCode)	\
	if (returnCode == false)			\
	{									\
		TestPassed = false;				\
	}

/*------------------------------------------------------------------------*/
/*---------------------   Data Types definitions   -----------------------*/
/*------------------------------------------------------------------------*/

typedef struct enumString
{
	int		enumID;
	int		enumValue;
	char	string[80];
} enumString;

/*------------------------------------------------------------------------*/
/*---------------------------   Externs   --------------------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*-----------------------   Function headers   ---------------------------*/
/*------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function:	PrintEnum													*/
/*                                                                          */
/* Parameters:  enumType	- Type/catergory of string to print				*/
/*				enumValue	- The ID Num of the string within the category	*/
/*																			*/
/* Returns:     Null terminated string										*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		This function is used to print predefined strings according to		*/
/*		a value. For example: Writing the reset type to string as follows:	*/
/*		In 'EnumType' (Utility.h), ENUM_RESET is defined.					*/
/*		In 'enumStringTable' (Utility.c), the following should be defined	*/
/*			ENUM_RESET,		RESET_VSB,		"VSB Reset",					*/
/*			ENUM_RESET,		RESET_VDD,		"VDD Reset",					*/
/*		PrintEnum() is used as follows:										*/
/*		  LogMessage("Reset Type = %s\n", PrintEnum(ENUM_RESET, RESET_VSB))	*/
/*		The results message is: "Reset Type = VSB Reset"					*/
/*		Enum_AssignTable() must be executed prior to PrintEnum()			*/
/*--------------------------------------------------------------------------*/
char*	PrintEnum						(int enumType, int enumValue);

/*--------------------------------------------------------------------------*/
/* Function:	Enum_AssignTable											*/
/*                                                                          */
/* Parameters:  enumStringTable	- The table which PrintEnum uses			*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		In order to be able to use PrintEnum(), a string table must be		*/
/*		specified. PrintEnum() searches this table for the matching string.	*/
/*--------------------------------------------------------------------------*/
void	Enum_AssignTable				(enumString* enumTable);

/*--------------------------------------------------------------------------*/
/* Function:	rand32														*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     Double-Word pseudo random number							*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Generates 32-bit of pseudo-random number instead of 15-bits			*/
/*--------------------------------------------------------------------------*/
DWORD	rand32							(void);

/*--------------------------------------------------------------------------*/
/* Function:	RandNonLinear												*/
/*                                                                          */
/* Parameters:  range	-	The range of values to randomize				*/
/*				factor	-	0 - Linear randomization						*/
/*							(each value has a equal chance to be randomized)*/
/*							> 0 - The randomization is not linear.			*/
/*							A greater value, means that high values			*/
/*							have lower chances to be randomized.			*/
/*																			*/
/* Returns:     Double-Word pseudo random number							*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		The randomization randomize values in a way that not all values		*/
/*		have equal chance to be selected. It is useful when testing some	*/
/*		configurations which should be tested only rarely, such as long		*/
/*		delays.																*/
/*--------------------------------------------------------------------------*/
//DWORD	RandNonLinear					(DWORD range, double factor);

/*--------------------------------------------------------------------------*/
/* Function:	Round														*/
/*                                                                          */
/* Parameters:  Number	-	Floating point number							*/
/*																			*/
/* Returns:     32-bit unsigned integer number 								*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		It accepts numbers with fractions and round it according to the		*/
/*		first digit right to the point. The output is an integer value.		*/
/*--------------------------------------------------------------------------*/
//DWORD	Round							(double Number);

/*--------------------------------------------------------------------------*/
/* Function:	RoundUp														*/
/*                                                                          */
/* Parameters:  Number	-	Floating point number							*/
/*																			*/
/* Returns:     32-bit unsigned integer number 								*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Always round a fractional number up, if the first digital right to	*/
/*		the point is 1 or greater.											*/
/*--------------------------------------------------------------------------*/
//DWORD	RoundUp							(double Number);

/*--------------------------------------------------------------------------*/
/* Function:	CompareWithTolerance										*/
/*                                                                          */
/* Parameters:  actualValue		-	First value, often the measured value	*/
/*				expectedValue	-	Second value, often the expected value	*/
/*																			*/
/* Returns:     0		-	First value is equal/near the second value		*/
/*							with the given tolerance.						*/
/*				1		-	First value is not within the tolerance,		*/
/*							compared to the second value.					*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Compares two numeric values, with a given tolerance.				*/
/*		If the value is lower or higher then the expected value including	*/
/*		the tolerance, the function returns false. If it is within the		*/
/*		tolerance, it returns true.											*/
/*		Examples:															*/
/*			CompareWithTolerance(107, 100, 5)								*/
/*			 - Return false, 107 value is not within the tolerance			*/
/*			CompareWithTolerance(96,  100, 5)								*/
/*			- Return true, 96 value is within the tolerance					*/
/*--------------------------------------------------------------------------*/
//int		CompareWithTolerance			(double actualValue, double expectedValue, double tolerance);

/*--------------------------------------------------------------------------*/
/* Function:	CompareWithToleranceP										*/
/*                                                                          */
/* Parameters:  actualValue		-	First value, often the measured value	*/
/*				expectedValue	-	Second value, often the expected value	*/
/*																			*/
/* Returns:     0		-	First value is equal/near the second value		*/
/*							with the given tolerance in percentage.			*/
/*				1		-	First value is not within the tolerance (in		*/
/*							percentage) compared to the second value.		*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Compares two numeric values, with a given a tolerance in percentage.*/
/*		See description of CompareWithTolerance() function					*/
/*		Examples:															*/
/*			CompareWithTolerance(1070, 100, 5)								*/
/*			 - Return false, 107 value is not within the tolerance			*/
/*			CompareWithTolerance(96,  100, 5)								*/
/*			- Return true, 96 value is within the tolerance					*/
/*--------------------------------------------------------------------------*/
//int		CompareWithToleranceP			(double actualValue, double expectedValue, double tolerance);

// * Timers and Delay *

/*--------------------------------------------------------------------------*/
/* Function:	Sleep														*/
/*                                                                          */
/* Parameters:  millisecond		-	Delay time in milliseconds				*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:Uses Yarkon's hardware timer								*/
/*																			*/
/* Description:                                                             */
/*		The function does not return until the defined number of			*/
/*		milliseconds had elapsed											*/
/*--------------------------------------------------------------------------*/
void	Sleep							(DWORD millisecond);

/*--------------------------------------------------------------------------*/
/* Function:	Delay														*/
/*                                                                          */
/* Parameters:  microsecond		-	Delay time in microseconds				*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:Uses Yarkon's hardware timer								*/
/*																			*/
/* Description:                                                             */
/*		The function does not return until the defined number of			*/
/*		microseconds had elapsed. The function is limited to 64,000us.		*/
/*--------------------------------------------------------------------------*/
void	Delay							(DWORD microsecond);

/*--------------------------------------------------------------------------*/
/* Function:	StartTimer													*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     >= 0	-	Handle to be used in other Timer APIs			*/
/*				 < 0	-	StartTimer failed								*/
/*																			*/
/* Side effects:Uses Yarkon's hardware timer								*/
/*																			*/
/* Description:                                                             */
/*		Allocate a timer, and run the timer (i.e., timer is counting)		*/
/*--------------------------------------------------------------------------*/
int		StartTimer (void);
void		InitTimers (void);
/*--------------------------------------------------------------------------*/
/* Function:	StopTimer													*/
/*                                                                          */
/* Parameters:  timer	-	Timer ID handle number (given by StartTimer)	*/
/*																			*/
/* Returns:     Time elapsed in milliseconds since StartTimer executed.		*/
/*																			*/
/* Side effects:Uses Yarkon's hardware timer								*/
/*																			*/
/* Description:                                                             */
/*		Returns the elapsed time since StartTimer executed in milliseconds,	*/
/*		stops and free the timer.											*/
/*--------------------------------------------------------------------------*/
DWORD	StopTimer						(int timer);

/*--------------------------------------------------------------------------*/
/* Function:	StopMicroTimer												*/
/*                                                                          */
/* Parameters:  timer	-	Timer ID handle number (given by StartTimer)	*/
/*																			*/
/* Returns:     Time elapsed in microseconds since StartTimer executed.		*/
/*																			*/
/* Side effects:Uses Yarkon's hardware timer								*/
/*																			*/
/* Description:                                                             */
/*		Returns the elapsed time since StartTimer executed in milliseconds,	*/
/*		stops and free the timer											*/
/*--------------------------------------------------------------------------*/
DWORD	StopMicroTimer					(int timer);

/*--------------------------------------------------------------------------*/
/* Function:	GetTimer													*/
/*                                                                          */
/* Parameters:  timer	-	Timer ID handle number (given by StartTimer)	*/
/*																			*/
/* Returns:     Time elapsed in milliseconds since StartTimer executed.		*/
/*																			*/
/* Side effects:Uses Yarkon's hardware timer								*/
/*																			*/
/* Description:                                                             */
/*		Returns the elapsed time since StartTimer executed in milliseconds.	*/
/*--------------------------------------------------------------------------*/
DWORD	GetTimer						(int timer);

/*--------------------------------------------------------------------------*/
/* Function:	GetMicroTimer												*/
/*                                                                          */
/* Parameters:  timer	-	Timer ID handle number (given by StartTimer)	*/
/*																			*/
/* Returns:     Time elapsed in microseconds since StartTimer executed.		*/
/*																			*/
/* Side effects:Uses Yarkon's hardware timer								*/
/*																			*/
/* Description:                                                             */
/*		Returns the elapsed time since StartTimer executed in microseconds.	*/
/*--------------------------------------------------------------------------*/
DWORD	GetMicroTimer					(int timer);

#endif /* _T_UTILITY_H_ */
