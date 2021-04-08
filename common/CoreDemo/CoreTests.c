/*--------------------------------------------------------------------------
 * Copyright (c) 2009-2010 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*-------------------------   Include files   ----------------------------*/
/*------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "../Arbel.h"
#include <PolegTemplate.h>
#include "Common.h"
#include "CoreGeneral.h"
#include "CoreUtility.h"
#include "CoreTests.h"
//#include "GIC/gic_API.h"

/*------------------------------------------------------------------------*/
/*----------------------   Constants definitions  ------------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*----------------------   Variable definitions   ------------------------*/
/*------------------------------------------------------------------------*/

BOOL	TestPassed;

/*------------------------------------------------------------------------*/
/*---------------------------   Externs   --------------------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*--------------------------   Functions   -------------------------------*/
/*------------------------------------------------------------------------*/

bool TEST_Configuration(void)
{
#if 0
	if (Template_Params.Test_Mode == POST_SILICON)
	{
		ID_PrintChipID();
		LogMessage("Post Silicon Test\n");

#if 0		
		// Configuring Interrupts & MIWU
		GIC_API_Init();
		GIC_API_Open();
		
		GIC_API_EnableAllInt();
#endif		
		MSG_TestDone();

	} 
	else
#endif
	{
		LogMessage("Palladium Silicon Test\n");
		// Configuring Interrupts & MIWU
#if 0
		GIC_API_Init();
		GIC_API_Open();
		GIC_API_EnableAllInt();
#endif
		ID_PrintChipID();
		MSG_TestDone();
	}
	
	LogMessage("TEST_Configuration Done");
	return TRUE;
}


bool TEST_Demonstration()
{
	/*----------------------------------*/
	/* Demonstrate logging capabilities */
	/*----------------------------------*/


	LogMessage("Core is ready to receive command!\n");
	while (MSG_RunTest() != TEST_LOG);

	LogHeader("This is a Message Box Demo");

	LogTitle("\nStarting test Demo (\"Log Title\")\n");
	LogMessage("\nThis is \"LogMessage\" message\n");
	//LogMessage("\nThis is \"LogMessage\" message with float %f\n",zozo);
	LogError("\nThis is \"LogError\" (failed test) message\n");
	LogPass("\nThis is \"LogPass\" (passed test) message\n\n");

// 	for (int percent = 0; percent <= 100; percent += 2)
// 	{
// 		LogProgress("Test Progress %d%%", percent);
// 		
// 	}
	LogMessage("\n\n");

	MSG_Data(0, 0x1234);

	MSG_TestDone();										// Signal to Host that TEST_LOG is done

	/*-----------------------------*/
	/* Receiving commands from Host */
	/*-----------------------------*/

	int		numOfTests = 0;

	LogHeader("Sending commands from Host to Core demonstration");

	while (numOfTests < 3)
	{
		// Note: It is highly recommended to place the code of the tests within 'CoreTests.cpp' file
		if (MSG_RunTest() == TEST_TEST1)
		{
			LogTitle("Test1 : Start running test...\n");

			// Get the Fast Read configuration
			LogMessage("PrintEnum() function demonstration\n");
		//	LogMessage("RD_MODE = %s\n", PrintEnum(ENUM_RD_MODE, GET_FIELD_REG(SPI_FL_CFG, 6, 7)));
			LogPass("Test1 : Test completed successfully!\n\n");
			MSG_TestDone();
			numOfTests++;
		}
		else
			if (MSG_RunTest() == TEST_TEST2)
			{
				LogTitle("Test2 : Start running test...\n");
				LogMessage("Waiting for 5 seconds");
// 				Sleep_ITIM8_ms(1000);								// Sleep for 1000ms
// 				LogMessage(".");
// 				for (int i = 1; i <= 20; i++)				// Sleep for 1 second total
					Delay(50000);							// Sleep for 50,000us each time
// 				LogMessage(".");
// 				Sleep_ITIM8_ms(1000);
// 				LogMessage(".");
// 				Sleep_ITIM8_ms(1000);
// 				LogMessage(".");
// 				Sleep_ITIM8_ms(1000);
// 				LogMessage(".\n");
				LogPass("Test2 : Test completed successfully!\n\n");
				MSG_TestDone();
				numOfTests++;
			}
			if (MSG_RunTest() == TEST_TEST3)
			{
				LogTitle("Test3 : Start running test...\n");
				LogMessage("Waiting for user to press a key\n");
				MSG_TestCommand(TCMD_KEYPRESS);
				while (MSG_val_TestCommand() == TCMD_KEYPRESS);	// Wait until Test Command is done
				LogMessage("User pressed the '%c' key\n", MSG_val_Data(0));
				LogPass("Test3 : Test completed successfully!\n\n");
				MSG_TestDone();
				//LogMessage("num of tests '%d' key\n", numOfTests);
				numOfTests++;
			}
	}
	return TRUE;
} // *End of TEST_Demonstration*


