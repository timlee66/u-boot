/*--------------------------------------------------------------------------
 * Copyright (c) 2009-2010 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*-------------------------   Include files   ----------------------------*/
/*------------------------------------------------------------------------*/
#include <stdlib.h>
#include <PolegTemplate.h>
#include "../Arbel.h"
#include "Common.h"
#include "CoreGeneral.h"
#include "CoreTests.h"
#include "../../arch/arm/include/asm/system.h"

BYTE channels_loop;


void core_peci_main(void)							// 'main' must be void
{
	MSG_Init(CORE_MSG_BA, CORE_MSG_SIZE);
	Enum_AssignTable(enumStringTablePtr);
	FuncPrintToLog = FALSE;									// Specify whether a function will display execution messages

//	while (MSG_RunTest() != TEST_ALL);						// Wait for host to tell core to start

	WRITE_REG(VSINTR, READ_REG(VSINTR) | 0x00000001);		// Enable VSINTEN to enable interrupt from PECI
	check_int();

	MSG_TestDone();
	while(1){
		wfi();	/* Wait for Interrupt */
	}											// Keep in endless loop since there is no OS to return to
} // *End of main*
