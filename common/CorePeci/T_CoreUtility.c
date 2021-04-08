/*--------------------------------------------------------------------------
 * Copyright (c) 2008-2009 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------
 * File Contents:
 *           Global utility functions.
 * Project : Yarkon
 * Author  : Shahar Ben-Varon
 *--------------------------------------------------------------------------*/
 
/*------------------------------------------------------------------------*/
/*-------------------------   Include files   ----------------------------*/
/*------------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/common.h"    /* rand */
#include "../Arbel.h"
#include "PolegT_Common.h"
#include "PolegT_CoreUtility.h"

/*------------------------------------------------------------------------*/
/*----------------------   Variable definitions   ------------------------*/
/*------------------------------------------------------------------------*/
static enumString*	enumStringTable = NULL;
BOOL		Timers[NUM_OF_TIMERS] = { false, false, false, false };

/*------------------------------------------------------------------------*/
/*---------------------------   Externs   --------------------------------*/
/*------------------------------------------------------------------------*/

char* PrintEnum(int enumType, int enumValue)
{
	int		strIndex = 0;

	// Check that enumStringTable points to an Enum table
	if (enumStringTable == NULL)
	{
		LogError("%s failed : Enum table is not assigned. Run Enum_AssignTable().\n", __FUNCTION__);
		return("PrintEnumError");
	}

	// Search for the matching string (which matches both enumType and enumValue) within the table
	do
	{
		if ((enumStringTable[strIndex].enumID	== enumType) &&
			(enumStringTable[strIndex].enumValue== enumValue))
		{
			return(enumStringTable[strIndex].string);
		}

		strIndex++;										// Next string in table
	} while(enumStringTable[strIndex].enumID != END_OF_TABLE);

	// If not string has been found, return an error
	LogError("%s failed : enumType or enumValue are invalid\n", __FUNCTION__);

	return("PrintEnumError");

} // *End of PrintEnum*

void Enum_AssignTable(enumString* enumTable)
{
	// Assign enumStringTable pointer with the user's table's address
	enumStringTable = enumTable;
}

DWORD rand32(void)
{
	return  (((DWORD)rand() << 30) + ((DWORD)rand() << 15) + (DWORD)rand());
}

#if 0
DWORD RandNonLinear(DWORD range, double factor)
{
	DWORD	rnd;										// Randomize value within the given range
	DWORD	chance;										// A chance that the randomized value will be selected

	while (true)
	{
		rnd = rand32() % range;							// Randomize a value within the range

		chance = (DWORD)((rnd * (rnd*factor)) + 1);		// Calculate the chance probability to accept the randomized value

		if ((rand32() % chance) == 0)					// If not zero, randomize another value which is lower than 'rnd'
			break;										// If the new value is zero, stop randomizing
	}

	return(rnd);										// Return the randomize value
} // *End of RandNonLinear*

DWORD Round(double Number)
{
	DWORD	Integer;

	Integer = (DWORD)(Number*10);
	if ((Integer % 10) >= 5)
		return((DWORD)Number+1);
	else
		return((DWORD)Number);
}

DWORD RoundUp(double Number)
{
	DWORD	Integer;

	Integer = (DWORD)(Number*10);
	if ((Integer % 10) >= 1)
		return((DWORD)Number+1);
	else
		return((DWORD)Number);
}

int CompareWithTolerance(double actualValue, double expectedValue, double tolerance)
{
	if (actualValue > expectedValue + tolerance)
		return(FUNC_FAILED);
	else
	if (actualValue < expectedValue - tolerance)
		return(FUNC_FAILED);
	else
		return(FUNC_PASSED);
} // *End of CompareWithTolerance*

int CompareWithToleranceP(double actualValue, double expectedValue, double tolerance)
{
	if (actualValue > expectedValue + tolerance * expectedValue)
		return(FUNC_FAILED);
	else
	if (actualValue < expectedValue - tolerance * expectedValue)
		return(FUNC_FAILED);
	else
		return(FUNC_PASSED);
} // *End of CompareWithToleranceP*
#endif
/*------------------------------------------------------------------------*/
/*-----------------------   Timers and Delay   ---------------------------*/
/*------------------------------------------------------------------------*/

#define	TIMER_PRESCALER					25

void InitTimers()
{	
	static BOOL init = FALSE;
	LogMessage("					InitTimers Init is %d\n",init);
	// Initialize only once
	if (!init)
	{
		//LogMessage("InitTimers()\n");

		// Set clock divider to 1
		SET_FIELD_REG(CLKDIV1, 21, 25, 0x00);			// Clear TIMCKDIV field

		// Set Timer to use 25MHz reference clock
		SET_FIELD_REG(CLKSEL, 14, 15, 0x2);				// Select 25MHz CLKREF

		// Don't perform initialization again
		init = TRUE;

		
	}
	for (int i = 0; i < NUM_OF_TIMERS; i++)
	{
		Timers[i] = FALSE;
	}
} // *End of InitTimers*

void Sleep(DWORD millisecond)
{	 
	DWORD i;

	if (millisecond == 0)
		return;

	InitTimers();

	WRITE_REG(TICR(4,1), millisecond * 1000);			// Timer load value (convert to 1us units)
	WRITE_REG(TCSR(4,1), BIT_MASK(30) | BIT_MASK(31) | TIMER_PRESCALER);// Start counting, prescalar 25 == 1 microseconds
	millisecond = READ_REG(TCSR(4,1));
	millisecond = READ_REG(TCSR(4,1));					// Issue bypass - Re-read
	millisecond = READ_REG(TCSR(4,1));					// Issue bypass - Re-read
	while (READ_REG(TCSR(4,1)) & BIT_MASK(25));			// Wait until not active*/

	millisecond *= 66685;								// Transform the delay into milliseconds
	millisecond /=16;
	for (i = 0; i < millisecond; i++);					// each iteration is ~18 cycles long
} // *End of Sleep*

void Delay(DWORD microsecond)
{
	DWORD i;

	if (microsecond == 0)
		return;

	InitTimers();
	
	WRITE_REG(TICR(4,1), microsecond);					// Timer load value
	WRITE_REG(TCSR(4,1), BIT_MASK(30) | BIT_MASK(31) | TIMER_PRESCALER);// Start counting, prescalar 25 == 1 microseconds
	microsecond = READ_REG(TCSR(4,1));
	microsecond = READ_REG(TCSR(4,1));					// Issue bypass - Re-read
	microsecond = READ_REG(TCSR(4,1));					// Issue bypass - Re-read
	while (READ_REG(TCSR(4,1)) & BIT_MASK(25));			// Wait until not active*/

	microsecond *= 66685;								// Transform the delay into microseconds
	microsecond /=16000;
	for (i = 0; i < microsecond; i++);					// Each iteration is ~18 cycles long
} // *End of Delay*

int StartTimer()
{
	int		timer;

	// Search for unused timer
	for (timer = 0; timer < NUM_OF_TIMERS; timer++)
	{
		if (Timers[timer] == FALSE)
		{
			Timers[timer] = TRUE;
			break;
		}
	}

	// Exit function if no available timer has been found
	if (timer >= NUM_OF_TIMERS)
	{
		LogMessage("StartTimer failed : All timers are occupied!\n");
		return(-1);
	}

	// Timer 0
	LogMessage("Timer selected %d\n ",timer);
	if (timer == 0)
	{
		WRITE_REG(TICR(0,1), TIMER_MAX_VALUE);			// Timer load value
		WRITE_REG(TCSR(0,1), BIT_MASK(30) | TIMER_PRESCALER);// Start counting, prescalar 24 == 1 microsecond
	}
	else
	// Timer 1
	if (timer == 1)
	{
		WRITE_REG(TICR(1,1), TIMER_MAX_VALUE);			// Timer load value
		WRITE_REG(TCSR(1,1), BIT_MASK(30) | TIMER_PRESCALER);// Start counting, prescalar 24 == 1 microsecond
	}
	else
	// Timer 2
	if (timer == 2)
	{
		WRITE_REG(TICR(2,1), TIMER_MAX_VALUE);			// Timer load value
		WRITE_REG(TCSR(2,1), BIT_MASK(30) | TIMER_PRESCALER);// Start counting, prescalar 24 == 1 microsecond
	}
	else
	// Timer 3
	if (timer == 3)
	{
		WRITE_REG(TICR(3,1), TIMER_MAX_VALUE);			// Timer load value
		WRITE_REG(TCSR(3,1), BIT_MASK(30) | TIMER_PRESCALER);// Start counting, prescalar 24 == 1 microsecond
	}

	// Return timer handler to caller
	return(timer);
} // *End of StartTimer*

DWORD _GetTimer(int timer)
{
	DWORD	time;
	
	// Exit function if no available timer has been found
	if (timer >= NUM_OF_TIMERS)
	{
		LogMessage("Timer failed : Illegal timer handler!\n");
		return((DWORD)-1);
	}
	
	// Get timer data
	time = READ_REG(TDR(timer,1));
	
	// Read up-to three time to make certain data is valid (data might not be valid)
	if ((time & ~0xFF) != (READ_REG(TDR(timer,1)) & ~0xFF))
		time = READ_REG(TDR(timer,1));
	
	// Convert time value to time elapsed
	time = TIMER_MAX_VALUE - time;

	return(time);
} // *End of _GetTimer*

DWORD _StopTimer(int timer)
{
	DWORD	time;

	time = _GetTimer(timer);
	
	// Mark timer an unused
	if (time != (DWORD)(-1))
		Timers[timer] = FALSE;

	return(time);
} // *End of ReadTime*

DWORD StopTimer(int timer)
{
	// Return the time (in milliseconds)
	return(_StopTimer(timer) / 1000);
} // *End of StopTimer*

DWORD StopMicroTimer(int timer)
{
	// Return the time (in microseconds)
	return(_StopTimer(timer));
}
DWORD GetTimer(int timer)
{
	// Return the time (in milliseconds)
	return(_GetTimer(timer) / 1000);
} // *End of GetTimer*

DWORD GetMicroTimer(int timer)
{
	// Return the time (in microseconds)
	return(_GetTimer(timer));
}

