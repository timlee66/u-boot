/*--------------------------------------------------------------------------
 * Copyright (c) 2009-2010 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*-------------------------   Include files   ----------------------------*/
/*------------------------------------------------------------------------*/

#include <stdlib.h>
#include <linux/math64.h>
#include <PolegTemplate.h>
#include "../Arbel.h"
#include "Common.h"
#include "CoreGeneral.h"
#include "PolegT_CoreUtility.h"

/*------------------------------------------------------------------------*/
/*----------------------   Constants definitions  ------------------------*/
/*------------------------------------------------------------------------*/

enumString	enumStringTable[] = {{END_OF_TABLE,NULL,""}};

/*------------------------------------------------------------------------*/
/*----------------------   Variable definitions   ------------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*---------------------------   Externs   --------------------------------*/
/*------------------------------------------------------------------------*/
enumString*	enumStringTablePtr = enumStringTable;
BOOL CorePrintFlag = TRUE;
/*------------------------------------------------------------------------*/
/*--------------------------   Functions   -------------------------------*/
/*------------------------------------------------------------------------*/

DWORD	Get_Random_Value(DWORD low_value, DWORD high_value)
{
	DWORD rnd_value;

	do 
	{
		rnd_value = rand32();
	} while(!((rnd_value >= low_value) && (rnd_value <= high_value)));

	return rnd_value;
}

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
/*void IRQ_TestHandler()
{
	// TODO: Add your interrupt service code here

} // *End of IRQ_TestHandler**/
