/*--------------------------------------------------------------------------
 * Copyright (c) 2008-2013 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*-------------------------   Include files   ----------------------------*/
/*------------------------------------------------------------------------*/
#include <stdlib.h>						// Used by srand()
#include <string.h>						// Used by strlen()
#include <stdio.h>
#include <stdarg.h>
//#include <asm.h>
#include "T_Common.h"
#include "PolegT_Common.h"
#include "PolegT_CoreCommon.h"
#include "PolegT_CoreUtility.h"
#include "../../include/common.h"    /* srand */

/*------------------------------------------------------------------------*/
/*----------------------   Constants definitions  ------------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*----------------------   Variable definitions   ------------------------*/
/*------------------------------------------------------------------------*/

#if 0
DWORD	OldIRQ = IRQ_NOT_INSTALLED;
void	(*IRQ_TestHandler)(void);
#endif
DWORD	MsgRegBA = 0;
BOOL	FuncPrintToLog = FALSE;

/*------------------------------------------------------------------------*/
/*---------------------------   Externs   --------------------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*--------------------------   Functions   -------------------------------*/
/*------------------------------------------------------------------------*/

/**************************************************************************/
/*******************************   IRQ   **********************************/
/**************************************************************************/
#if 0
/*--------------------------------------------------------------------------*/
/* Function:	IRQ_Handler													*/
/*                                                                          */
/* Parameters:  None														*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:Interrupt handler, called a generation of an interrupt		*/
/*																			*/
/* Description:                                                             */
/*		Handles the interrupt and call a User service test function			*/
/*--------------------------------------------------------------------------*/
__irq void IRQ_Handler(void)
{
	DWORD	vectNum;					// Interrupt number

	vectNum = READ_REG(AIC_IPER);		// Just read from AIC_IPER
    vectNum = READ_REG(AIC_ISNR);
    WRITE_REG(AIC_SCCR, (1 << vectNum));

	// Call the user IRQ handler
	IRQ_TestHandler();

	WRITE_REG(AIC_EOSCR, 0x00);			// Write any value to permit service of pending interrupts
} // *End of IRQ_Handler*

/*--------------------------------------------------------------------------*/
/* Function:	IRQ_InstallHandler											*/
/*                                                                          */
/* Parameters:  interruptNum - Interrupt number								*/
/*				IRQ_UserHandler - The address of the IRQ handler function	*/
/*																			*/
/* Returns:     None														*/
/*																			*/
/* Side effects:The interrupt service routine will be called upon interrupt	*/
/*																			*/
/* Description:                                                             */
/*		This function installs an interrupt servce.							*/
/*		The test should add a interrupt handling code to IRQ_TestHandler()	*/
/*--------------------------------------------------------------------------*/
void IRQ_InstallHandler(BYTE interruptNum, void (*IRQ_UserHandler)(void))
{
	int		_cpsr;						// Current value of CPSR register

	FUNC_PRINT(("IRQ_InstallHandler(interruptNum = %d)\n", interruptNum));

	// Save the current interrupt
	OldIRQ = *(volatile WORD*)0x38;
	*((volatile DWORD*)0x38) = (DWORD)IRQ_Handler;

	// Configure the interrupt
	WRITE_REG(AIC_SCCR, 0xFFFFFFFF);					// Clear all interrupts
	WRITE_REG(AIC_MDCR, 0xFFFFFFFF);					// Disable all interrupts
	WRITE_REG(AIC_MECR, 1 << interruptNum);				// Enable interrupt

	// Enable interrupts within the ARM
	// Enable CPSR 'I' bit
	__asm
	{
		MRS    _cpsr, CPSR
		BIC    _cpsr, _cpsr, 0x80
		MSR    CPSR_c, _cpsr
	}

	// Set the user interrupt handler, which is called by IRQ_Handler
	IRQ_TestHandler = IRQ_UserHandler;
} // *End of IRQ_InstallHandler*

void IRQ_RemoveHandler()
{
	FUNC_PRINT(("IRQ_RemoveHandler()\n"));

	// Check if IRQ handler has been installed
	if (OldIRQ == IRQ_NOT_INSTALLED)
	{
		LogError("IRQ_RemoveHandler failed : No IRQ handler has been installed!\n");
		return;
	}

	// Restore old interrupt handler
   	*(volatile DWORD*)0x38 = OldIRQ;
	OldIRQ = NULL;
} // *End of IRQ_RemoveHandler*
#endif
/**************************************************************************/
/**************************   Core Related   ******************************/
/**************************************************************************/

void Core_CacheEnable()
{
// 	volatile WORD	saved_psr = 0;    // PSR register is read into this variable
// 
// 	FUNC_PRINT(("%s()\n", __FUNCTION__));
// 
// 	_spr_("cfg",saved_psr);
// 	saved_psr |= 0x10;
// 	_lpr_("cfg",saved_psr);
}

void Core_CacheDisable()
{
// 	volatile WORD	saved_psr = 0;    // PSR register is read into this variable
// 
// 	FUNC_PRINT(("%s()\n", __FUNCTION__));
// 
// 	_spr_("cfg",saved_psr);
// 	saved_psr &= (~0x10);
// 	_lpr_("cfg",saved_psr);
}

/**************************************************************************/
/**********************   Host/Core Message Box   *************************/
/**************************************************************************/

void MSG_Init(DWORD msgRegBA, WORD msgAddrSpaceSize)
{
	
	DWORD	randomSeed;
	
	/*------------------------------------------------------------------*/
	/* Assign Message Registers base address supplied by the user		*/
	/*------------------------------------------------------------------*/
/*
	// There is no point of returning an error, since the Host will not be notified
	if (msgAddrSpaceSize < MSG_REG_SIZE + MSG_MIN_BUF_SIZE)
		return(-1);
*/
	// Assign base address
	MsgRegBA = msgRegBA;

	/*------------------------------------------------------------------*/
	/* Clear all Message Box registers									*/
	/*------------------------------------------------------------------*/

	// Backup the random seed value
	randomSeed = READ_REG(msgRandomSeed);

	// Zero all Message Box registers
	memset((void*)(uintptr_t)MsgRegBA, 0x00, MSG_REG_SIZE);
	
	// Set buffer size (the remainder after subtracting the registers space)
	WRITE_REG(msgBufferSize, msgAddrSpaceSize - MSG_REG_SIZE);

	// Set the Commands done bits (cleared bits can cause MSG_WaitUntilReadBuHost() getting stuck)
	SET_BIT_REG(msgStatus, MSG_COMMAND_DONE);
	SET_BIT_REG(msgStatus, MSG_TCOMMAND_DONE);

	/*------------------------------------------------------------------*/
	/* Setup random seed taken from Host								*/
	/*------------------------------------------------------------------*/

	// Set random seed value given by the Host
	WRITE_REG(msgRandomSeed, randomSeed);
	srand(READ_REG(msgRandomSeed));

} // *End of MSG_Init*

DWORD MSG_RunTest()
{
	return(READ_REG(msgRunTest));
}

void MSG_Command(DWORD command)
{
	FUNC_PRINT(("Core: MSG_Command(command = %d)\n", command));
//	MSG_WaitUntilReadByHost();							// Wait until Host prints all text messages

	WRITE_REG(msgCommand, command);						// Request from Core to run a specific test/command
	CLEAR_BIT_REG(msgStatus, MSG_COMMAND_DONE);			// Clear Command Done bit, i.e., command not done

	// Wait only if 'CMD_NO_WAIT' isn't set
	if (!READ_BIT_REG(msgParameters, MSG_PARAM_CMD_NO_WAIT))
	{
		// Wait until command has been executed by the Host
		while (!READ_BIT_REG(msgStatus, MSG_COMMAND_DONE));
	}
	FUNC_PRINT(("MSG_Command(command = %d) Host Completed\n", command));
} // *End of MSG_Command*

DWORD MSG_val_Command(void)
{
	return(READ_REG(msgCommand));						// Returns the current value of msgCommand register
}

void MSG_TestCommand(DWORD command)
{
	FUNC_PRINT(("MSG_TestCommand(command = %d)\n", command));

	WRITE_REG(msgTestCommand, command);					// Request from Core to run a specific test/command
	CLEAR_BIT_REG(msgStatus, MSG_TCOMMAND_DONE);		// Clear Test Command Done bit, i.e., command not done

	// Wait only if 'CMD_NO_WAIT' isn't set
	if (!READ_BIT_REG(msgParameters, MSG_PARAM_CMD_NO_WAIT))
	{
		// Wait until command has been executed by the Host
		while (!READ_BIT_REG(msgStatus, MSG_TCOMMAND_DONE));
	}
}

DWORD MSG_val_TestCommand()
{
	return(READ_REG(msgTestCommand));
}

void MSG_TestDone()
{
	// Zero Command register
	WRITE_REG(msgRunTest, 0);

	// Signal Tests Done!
	SET_BIT_REG(msgStatus, MSG_TEST_DONE);
}

void MSG_CommandsNoWait(bool enable)
{
	if (enable)
		SET_BIT_REG(msgParameters, MSG_PARAM_CMD_NO_WAIT);
	else
		CLEAR_BIT_REG(msgParameters, MSG_PARAM_CMD_NO_WAIT);
}

#if 0
int MSG_FunctionCall(DWORD command, DWORD param1, DWORD param2, DWORD param3, DWORD param4, DWORD param5, DWORD param6, DWORD param7, DWORD param8)
{
	int		timer;						// Timer handler for time-out check

	WRITE_REG(msgCmdParam1, param1);
	WRITE_REG(msgCmdParam2, param2);
	WRITE_REG(msgCmdParam3, param3);
	WRITE_REG(msgCmdParam4, param4);
	WRITE_REG(msgCmdParam5, param5);
	WRITE_REG(msgCmdParam6, param6);
	WRITE_REG(msgCmdParam7, param7);
	WRITE_REG(msgCmdParam8, param8);

	CLEAR_BIT_REG(msgStatus, MSG_COMMAND_DONE);
	WRITE_REG(msgCommand, command);

	// Wait up to one second for command to complete
	timer = StartTimer();
	while (GetTimer(timer) < ONE_SECOND)
	{
		if (READ_BIT_REG(msgStatus, MSG_COMMAND_DONE))
		{
			StopTimer(timer);
			return(FUNC_PASSED);
		}
	}

	StopTimer(timer);

	LogError("MSG_FunctionCall failed : Host did not complete the command\n");

	return(FUNC_FAILED);
} // *MSG_FunctionCall*
#endif

void MSG_Data(BYTE dataNum, DWORD value)
{
	if (dataNum >= MSG_DATA_NUM)
	{
		LogError("%s failed : dataNum = %d is invalid. dataNum must be <= %d\n", __FUNCTION__, dataNum, MSG_DATA_NUM);
		return;
	}
	
	// Write the value to the corresponding dataNum
	WRITE_REG(msgData(dataNum), value);
} // *MSG_Data*

DWORD MSG_val_Data(BYTE dataNum)
{
	if (dataNum >= MSG_DATA_NUM)
	{
		LogError("%s failed : dataNum = %d is invalid. dataNum must be <= %d\n", __FUNCTION__, dataNum, MSG_DATA_NUM);
		return(TEST_FAILED);
	}
	
	// Write the value to the corresponding dataNum
	return(READ_REG(msgData(dataNum)));
} // *MSG_Data*

void MSG_RemoveLineFeed(char* message)
{
	int		offset = 0;
	int		newOffset = 0;

	/*------------------------------------------------------------------*/
	/* Copy all characters except of newline and linefeed				*/
	/*------------------------------------------------------------------*/
	do
	{
		// Copy all characters except of newline and linefeed
		if ((message[offset] != (char)0x0A) && (message[offset] != (char)0x0D))
		{
			message[newOffset] = message[offset];
			newOffset++;
		}
		offset++;
	} while ((message[offset] != 0) && (offset < MSG_MAX_SIZE-1));

	message[newOffset] = 0;
} // *End of MSG_RemoveLineFeed*

void MSG_WaitUntilReadByHost()
{
	// Wait until the Host read the text buffer and no commands are pending
	while ((msgTxtHostPos != msgTxtCorePos) && (!READ_BIT_REG(msgStatus, MSG_COMMAND_DONE)) && (!READ_BIT_REG(msgStatus, MSG_TCOMMAND_DONE)));
}
#if 0
void AlignNumericString(char *String, BYTE Align, BOOL ZeroFill)
{
	char	TempString[20];								// Temporary string, used for recreation of the string
	BYTE	Index;										// Current processed character

	// Exit this function if no alignment is required
	if (strlen(String) >= Align)
		return;

	// Zero the Output string
	memset(TempString, 0x00, sizeof(TempString));

	// Add characters before the numeric string
	for (Index = 0; Index < Align-strlen(String); Index++)
	{
		if (ZeroFill)
			TempString[Index] = '0';					// Fill with zeros
		else
			TempString[Index] = ' ';					// Fill with spaces
	}

	strcat(TempString, String);							// Append the original number
	strcpy(String, TempString);							// Store OutputString in String
} // *End of AlignNumericString*

void itoa(DWORD value, char *string, int radix)
{
	char	Hexdecimal[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	char	String[15] = "";							// Stores the ASCII number
	BYTE	Index;										// Current character within the string
	BYTE	StringLength;								// The length of the string

	// Convert the numeric number to string
	Index = 0;
	do
	{
		String[Index] = Hexdecimal[value % radix];		// Calculate the digit
		value /= radix;
		Index++;										// Next character
	}
	while (value != 0);									// When the number reaches 0, stop
	
	// Reverse the order of the character in the string
	StringLength = Index-1;
	for (Index = 0; Index <= StringLength; Index++)
		string[Index] = String[StringLength-Index];
	string[Index] = 0x00;								// Make sure that the string is NULL terminated
} // *End of itoa*

void strupr(char *String)
{
	int		Index;										// Current processed character
	
	Index = 0;
	while (String[Index] != NULL)
	{
		if ((String[Index] >= 'a') && (String[Index] <= 'f'))
			String[Index] -= 0x20;						// Convert character to lower case
		Index++;
	}
} // *End of strupr*

void PrintToString(char *String, const char *fmt, va_list args)
{
	DWORD	d;											// Used in %d
	DWORD	x;											// Used in %x and %X
	char	*s;											// Used in %s
	char	Align;										// Specify to how many character to align. e.g. '    27h' (0 means no alignment)
	BOOL	ZeroFill;									// Specify if zeroes are inserted instead of spaces. e.g. '000027h'
	char	d_str[15];									// Stores the string from d variable
	char	x_str[10];									// Stores the string from x variable
	char	OutputString[MSG_MAX_SIZE];					// The final string after reformatting
	BYTE	OutputOffset, InputOffset;					// Current character processed in Output and Input strings
	
	memset(OutputString, 0x00, sizeof(OutputString));

	OutputOffset = 0;
	for (InputOffset = 0; fmt[InputOffset] != 0x00; InputOffset++)
	{
		// Search for %. Else, copy the character as it is
		if (fmt[InputOffset] != '%')
		{
			OutputString[OutputOffset] = fmt[InputOffset];
			OutputOffset++;
		}
		else
		{
			InputOffset++;								// Skip '%' character

			// Does the number is aligned with zero fill ?
			if (fmt[InputOffset] == '0')
			{
				ZeroFill = TRUE;
				InputOffset++;							// Skip '0' character
			}
			else
				ZeroFill = FALSE;						// ZeroFill is disabled by default

			// Does the number has to be aligned ?
			if ((fmt[InputOffset] >= '1') && (fmt[InputOffset] <= '9'))
			{
				Align = fmt[InputOffset] - '0';			// Keep the number of character to align to
				InputOffset++;							// Next character
			}
			else
				Align = 0;								// No alignment

			// Is the variable is a long int or short int ?
			if (fmt[InputOffset] == 'l')
			{
				InputOffset++;							// Skip the 'l' character
				d = x = va_arg(args, DWORD);			// 32-bit
			}
			else
			if (!(fmt[InputOffset] ==  's'))			// Get the next parameter only if it is not a string
				d = x = va_arg(args, WORD);				// 16-bit

			// Check all flags
			if ((fmt[InputOffset] == 'd') || (fmt[InputOffset] == 'i'))
			{
				itoa(d, d_str, 10);
				AlignNumericString(d_str, Align, ZeroFill);
				strcat(OutputString, d_str);
				OutputOffset += strlen(d_str);
			}
			else
			if ((fmt[InputOffset] == 'x') || (fmt[InputOffset] == 'l'))
			{
				itoa(x, x_str, 16);
				AlignNumericString(x_str, Align, ZeroFill);
				strcat(OutputString, x_str);
				OutputOffset += strlen(x_str);
			}
			else
			if (fmt[InputOffset] ==  'X')
			{
				itoa(x, x_str, 16);
				strupr(x_str);
				AlignNumericString(x_str, Align, ZeroFill);
				strcat(OutputString, x_str);
				OutputOffset += strlen(x_str);
			}
			else
			if (fmt[InputOffset] ==  's')
			{
				s = va_arg(args, char*);
				strcat(OutputString, s);
				OutputOffset += strlen(s);
			}
		}
	}
	strcpy(String, OutputString);
} // *End of PrintToString*
#endif
void PrintMsg(MsgType MessageType, char *fmt, va_list args)
{
	char	Buf[MSG_MAX_SIZE];
	char	*BufPtr;
	BYTE	MsgLen;
	
	// Return immediately if string is NULL
	if (fmt == NULL)
		return;

	// Return immediately if Silent mode is enabled
	if (BIT_IS_SET(msgParameters, MSG_PARAM_SILENT))	// Do not send message in silent mode
		return;

//	vsnprintf(Buf, MSG_MAX_SIZE, fmt, args);			// Save the formatted string in Buf (truncate string if exceed limits)
	vsprintf(Buf, fmt, args);							// Save the formatted string in Buf (truncate string if exceed limits)
//	PrintToString(Buf, fmt, args);						// Save the formatted string in Buf
	MsgLen = strlen(Buf);								// Measure the length of the string + End (or not) of buffer indicator
	
	// Is this string is small enough to fit into the end of the MessageBuffer ?
	if ((msgTxtCorePos + MsgLen) >= msgBufferSize-1)
	{													// No !
		SET_BIT_REG(msgStatus, MSG_BUFFER_FULL);		// Set the BUFFER_FULL bit to signal the Host that the buffer is full
		//SET_BIT_REG(msgStatus,MSG_TRIGGER);
		while (msgTxtCorePos != msgTxtHostPos);
		msgTxtCorePos = 0;								// Set pointers to the beginning of the buffer
		msgTxtHostPos = 0;
		CLEAR_BIT_REG(msgStatus, MSG_BUFFER_FULL);		// Clear BUFFER_FULL bit
		
	}

	BufPtr = (char*)(uintptr_t)(MESSAGE_BUFFER + msgTxtCorePos);	// Set BufPtr to point to the current available space in MESSAGE_BUFFER
	*BufPtr = MessageType;								// Write to MESSAGE_BUFFER the type of the message
	*(BufPtr+1) = MsgLen;								// Write to MESSAGE_BUFFER the length of the string
	strcpy((BufPtr+2), Buf);							// Write the string to MESSAGE_BUFFER
	msgTxtCorePos += MsgLen+2;							// Set msgCorePos to point to the next free address (MessageSize+MessageLength+MessageType)

	// If WAIT4HOST parameter is set, wait until each message is read by the Host
	if (BIT_IS_SET(msgParameters, MSG_PARAM_WAIT4HOST))
		MSG_WaitUntilReadByHost();

	/*------------------------------------------------------------------*/
	/* Use standard printf() when a debugger is used.					*/
	/* printf() function must not be included in the code (even if not	*/
	/* executed) when compiling for template mode. If an I/O function is*/
	/* somewhere in the code (even at non accessible function), the code*/
	/* will hang on endless loop during the stack initialization.		*/
	/*------------------------------------------------------------------*/
#ifdef DEBUGGER
	MSG_RemoveLineFeed(Buf);

	if (MessageType == MSG_NORMAL)
		printf("%s\n", Buf);
	else
	if (MessageType == MSG_PASS)
		printf("PASSED: %s\n", Buf);
	else
	if (MessageType == MSG_FAIL)
		printf("ERROR: %s\n", Buf);
	else
	if (MessageType == MSG_TITLE)
		printf("* %s *\n", Buf);
	else
	if (MessageType == MSG_HEADER)
		printf("*** %s ***\n", Buf);

	// Do not advance the pointer (otherwise, the buffer will be come full and the function will stuck)
	msgTxtCorePos = 0;
#endif
} // *End of PrintMsg

void LogMessage(char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);								// Initialize variable arguments
	PrintMsg(MSG_NORMAL, fmt, args);
}

void LogPass(char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);								// Initialize variable arguments
	PrintMsg(MSG_PASS, fmt, args);
}

void LogError(char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);								// Initialize variable arguments
	PrintMsg(MSG_FAIL, fmt, args);
}

void LogTitle(char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);								// Initialize variable arguments
	PrintMsg(MSG_TITLE, fmt, args);
}

void LogHeader(char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);								// Initialize variable arguments
	PrintMsg(MSG_HEADER, fmt, args);
}

void LogProgress(char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);								// Initialize variable arguments
	PrintMsg(MSG_PROGRESS, fmt, args);
}

void LogToString(char* Output, char *fmt, ...)
{
	va_list	args;

	va_start(args, fmt);								// Initialize variable arguments
	//PrintToString(Output, fmt, args);
	vsprintf(Output, fmt, args);
}
