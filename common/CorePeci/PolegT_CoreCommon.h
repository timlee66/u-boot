/*-------------------------------------------------------------------------
 * Copyright (c) 2008-2009 by Nuvoton Technology Israel
 * All rights reserved.
 *------------------------------------------------------------------------*/
#ifndef __T_CORE_COMMON__
#define __T_CORE_COMMON__

//#include <stdio.h>
#include <tl_common.h>

/*------------------------------------------------------------------------*/
/*----------------   Constants and macros definitions  -------------------*/
/*------------------------------------------------------------------------*/

#define true							1
#define false							0

#define IRQ_NOT_INSTALLED				0xDEADBEEF

/*------------------------------------------------------------------------*/
/*---------------------   Data Types definitions   -----------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*---------------------------   Externs   --------------------------------*/
/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
/*-----------------------   Function headers   ---------------------------*/
/*------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function:	IRQ_InstallHandler											*/
/*                                                                          */
/* Parameters:  Interrupt number											*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:The interrupt service routine will be called upon interrupt	*/
/*																			*/
/* Description:                                                             */
/*		This function installs an interrupt servce.							*/
/*		The test should add a interrupt handling code to IRQ_TestHandler()	*/
/*--------------------------------------------------------------------------*/
void	IRQ_InstallHandler				(BYTE interruptNum, void (*IRQ_UserHandler)(void));

/*--------------------------------------------------------------------------*/
/* Function:	IRQ_RemoveHandler											*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:No service routine will be called upon an interrupt			*/
/*																			*/
/* Description:                                                             */
/*		This function reverse the IRQ handler configuration done by			*/
/*		IRQ_InstallHandler(). This function should not be called if			*/
/*		IRQ_InstallHandler() wasn't called first.							*/
/*--------------------------------------------------------------------------*/
void	IRQ_RemoveHandler				(void);

/*--------------------------------------------------------------------------*/
/* Function:	Core_CacheEnable											*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Enables Core's Instruction Cache.									*/
/*--------------------------------------------------------------------------*/
void	Core_CacheEnable					(void);

/*--------------------------------------------------------------------------*/
/* Function:	Core_CacheDisable											*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Disables Core's Instruction Cache.									*/
/*--------------------------------------------------------------------------*/
void	Core_CacheDisable					(void);

/*--------------------------------------------------------------------------*/
/* * Message functions *													*/
/*--------------------------------------------------------------------------*/
/* The message functions are used for mainly sending text messages from Core*/
/* machine to Host machine (i.e., from Core to PC).							*/
/* This mechanism must be first initialized using MSG_Init().				*/
/* There are several logging functions which are TestEC compatible, such as	*/
/* LogMessage, LogPass and LogError. There are some other logging functions	*/
/* which does not have a corresponding TestEC function.						*/
/* Each function, call from the Core, writes the message to a buffer. The	*/
/* host is checking (using another set of functions) whether a message is	*/
/* avaiable and process it. Due to the slow JTAG interface if Core, this	*/
/* communication mechanism is relatively slow. Sending a message can take	*/
/* ~200ms for each text line.												*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function:	MSG_Init													*/
/*                                                                          */
/* Parameters:  msgRegBA	- Base Address for storing the Messaging		*/
/*							  System internal registers. This base address	*/
/*							  must be compatible with linkerobj.def file	*/
/*				msgAddrSpaceSize - The allocated address space for the		*/
/*							  Message system. Must be at least 280h			*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:Clears all messages and its registers						*/
/*																			*/
/* Description:                                                             */
/*		Initialize the Text Message Box between Host and Core.				*/
/*		Must be called only once, before any other MSG functions.			*/
/*--------------------------------------------------------------------------*/
void	MSG_Init						(DWORD msgRegBA, WORD msgAddrSpaceSize);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_RunTest													*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     Current Test ID value										*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Reads the current value of the Test ID.								*/
/*--------------------------------------------------------------------------*/
DWORD	MSG_RunTest						(void);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_Command													*/
/*                                                                          */
/* Parameters:  command	-	Command to execute (see CMD_ENUM)				*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Sends the Core a specific Command, as specified in CMD_ENUM.		*/
/*		The commands are project specific and can be customized.			*/
/*--------------------------------------------------------------------------*/
void	MSG_Command						(DWORD command);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_val_Command													*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     Current Command value										*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Reads the current value of the Command.								*/
/*--------------------------------------------------------------------------*/
DWORD	MSG_val_Command						(void);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_TestDone												*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:Sets the RunTest ID to NULL									*/
/*																			*/
/* Description:                                                             */
/*		Signal the Host that a test has been completed.						*/
/*		This causes the Host to stop waiting for messages.					*/
/*		Signaling that a test is DONE, can be done within the code in		*/
/*		several different places, which means that using this function is	*/
/*		not a must.															*/
/*--------------------------------------------------------------------------*/
void	MSG_TestDone					(void);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_CommandsNoWait											*/
/*                                                                          */
/* Parameters:  enable	- Core doesn't wait for host to complete commands	*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:															*/
/*																			*/
/* Description:                                                             */
/*		When enabled, the Core does not wait until the Host finish			*/
/*		executing a Command or Test Command. The Core continues without		*/
/*		wait.																*/
/*--------------------------------------------------------------------------*/
void	MSG_CommandsNoWait				(bool enable);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_FunctionCall											*/
/*                                                                          */
/* Parameters:  command	-	Function/Command ID to execute					*/
/*																			*/
/* Returns:     true	-	Function returned properly						*/
/*				false	-	Function did not return on time (host did not	*/
/*							complete executing the function on a reasonable	*/
/*							time.											*/
/*																			*/
/* Side effects:Destroys command and paramters registers					*/
/*																			*/
/* Description:                                                             */
/*		Command the Host to perform a certain function given specified		*/
/*		parameters and getting back parameters in a function call like		*/
/*		method.																*/
/*--------------------------------------------------------------------------*/
//int		MSG_FunctionCall				(DWORD command, DWORD param1 = NULL, DWORD param2 = NULL, DWORD param3 = NULL, DWORD param4 = NULL, DWORD param5 = NULL, DWORD param6 = NULL, DWORD param7 = NULL, DWORD param8 = NULL);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_Data													*/
/*                                                                          */
/* Parameters:  dataNum	-	Data index number to write to (0-9).			*/
/*				value	-	Value to write to the specific data index.		*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Write values to semaphore data registers. There are 10 registers.	*/
/*--------------------------------------------------------------------------*/
void	MSG_Data						(BYTE dataNum, DWORD value);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_val_Data													*/
/*                                                                          */
/* Parameters:  dataNum	-	Data index number to read from (0-9).			*/
/*																			*/
/* Returns:     The value stored in the requested data register.			*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Reads values from semaphore data registers. There are 10 registers.	*/
/*--------------------------------------------------------------------------*/
DWORD	MSG_val_Data						(BYTE dataNum);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_TestCommand												*/
/*                                                                          */
/* Parameters:  command	-	Command to execute (see TCMD enum)				*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Sends the Core a specific TestCommand, as specified TCMD enum.		*/
/*		The commands are test specific and can be customized for each test.	*/
/*--------------------------------------------------------------------------*/
void	MSG_TestCommand					(DWORD command);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_val_TestCommand												*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     Current Test Command value									*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Reads the current value of the TestCommand.							*/
/*--------------------------------------------------------------------------*/
DWORD	MSG_val_TestCommand					(void);

/*--------------------------------------------------------------------------*/
/* Function:	LogMessage													*/
/*                                                                          */
/* Parameters:  Same as printf() parameters									*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Sends a text message to Host and ask the Host to format it as		*/
/*		a LogMessage() text format.											*/
/*--------------------------------------------------------------------------*/
void	LogMessage						(char *fmt, ...);

/*--------------------------------------------------------------------------*/
/* Function:	LogPass														*/
/*                                                                          */
/* Parameters:  Same as printf() parameters									*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Sends a text message to Host and ask the Host to format it as		*/
/*		a LOG_PASS() text format, i.e., Green text and increase total number*/
/*		of tests counter.													*/
/*--------------------------------------------------------------------------*/
void	LogPass							(char *fmt, ...);

/*--------------------------------------------------------------------------*/
/* Function:	LogError													*/
/*                                                                          */
/* Parameters:  Same as printf() parameters									*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Sends a text message to Host and ask the Host to format it as		*/
/*		a LOG_ERROR() text format, i.e., Red text and increase number		*/
/*		of failed tests counter.											*/
/*--------------------------------------------------------------------------*/
void	LogError						(char *fmt, ...);

/*--------------------------------------------------------------------------*/
/* Function:	LogTitle													*/
/*                                                                          */
/* Parameters:  Same as printf() parameters									*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Sends a text message to Host and ask the Host to format it as		*/
/*		a Yellow text message (correposnds to COLOR_LOG(YELLOW, ("Text")).	*/
/*--------------------------------------------------------------------------*/
void	LogTitle						(char *fmt, ...);

/*--------------------------------------------------------------------------*/
/* Function:	LogHeader													*/
/*                                                                          */
/* Parameters:  Same as printf() parameters									*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:None														*/
/*																			*/
/* Description:                                                             */
/*		Sends a text message to Host and ask the Host to format it in a		*/
/*		frame made of asterisk (*). This is not TestEC compatible function.	*/
/*--------------------------------------------------------------------------*/
void	LogHeader						(char *fmt, ...);

/*--------------------------------------------------------------------------*/
/* Function:	LogProgress													*/
/*                                                                          */
/* Parameters:  Same as printf() parameters									*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:Overwrites the text in the current line						*/
/*																			*/
/* Description:                                                             */
/*		Sends a text message to Host and ask the Host to delete the current	*/
/*		text and overwrite it with LogProgress' text. This is useful to		*/
/*		progress status on the same line each time, such as current			*/
/*		processed address or compltetion in percentage.						*/
/*--------------------------------------------------------------------------*/
void	LogProgress						(char *fmt, ...);

/*--------------------------------------------------------------------------*/
/* Function:	MSG_WaitUntilReadByHost										*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:Holds the Core in a loop									*/
/*																			*/
/* Description:                                                             */
/*		Waits until all text messages and commands have been read by the  	*/
/*		Host and the message buffer is completely free.						*/
/*--------------------------------------------------------------------------*/
void	MSG_WaitUntilReadByHost			(void);

#endif // __T_CORE_COMMON__
