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
#include "CoreTests.h"
#include "../armv8_interrupts/gic_v2.h"




volatile int PECI_Interrupt=0;
volatile int Int_Occured=0;


void IRQ_PECI_Handler(void) {
	LogMessage("Peci handler\n");
	PECI_Interrupt = PECI_INT;
	Int_Occured = 1;
	WRITE_REG(PECI_CTL_STS, DONE);								// Clear the DONE indication and close DONE_EN
}



void check_int(void) {

	gic_v2_initialize(PECI_INT);
	
	LogTitle("\nTesting interrupt generation from the PECI module:\n");
	LogTitle("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	// interrupts disabled
	WRITE_REG(PECI_CTL_STS, DONE);										// Clear DONE status turn off DONE_EN
	WRITE_REG(PECI_ADDR, DEVICE_ADDR);
	WRITE_REG(PECI_WR_LENGTH, 0x01);
	WRITE_REG(PECI_RD_LENGTH, 0x02);
	WRITE_REG(PECI_CMD, GET_TEMP_0);
	PECI_Interrupt = Int_Occured = 0;
	enable_irq();
	LogMessage("Generating PECI transaction...\n");
	WRITE_REG(PECI_CTL_STS, DONE_EN | START_BUSY);						// Start the transaction and turn on interrupts
	while (((READ_REG(PECI_CTL_STS) & DONE) != DONE) && (Int_Occured == 0x00));
	if (PECI_Interrupt == PECI_INT) LogPass("Pass: Got correct interrupt (#%d) after transaction when interrupt enabled\n", PECI_INT);
	else if (PECI_Interrupt == 0) LogError("Error: Did not get interrupt after transaction when interrupt enabled\n");
	else LogError("Error: Got incorrect interrupt from PECI (got #%d instead of #%d)\n", PECI_Interrupt, PECI_INT);

	// interrupts disabled
	WRITE_REG(PECI_CTL_STS, DONE);										// Clear DONE status turn off DONE_EN
	WRITE_REG(PECI_ADDR, DEVICE_ADDR);
	WRITE_REG(PECI_WR_LENGTH, 0x01);
	WRITE_REG(PECI_RD_LENGTH, 0x02);
	WRITE_REG(PECI_CMD, GET_TEMP_0);
	PECI_Interrupt = Int_Occured = 0;
	LogMessage("Generating PECI transaction...\n");
	WRITE_REG(PECI_CTL_STS, START_BUSY);								// Start the transaction and turn off interrupts
	while (((READ_REG(PECI_CTL_STS) & DONE) != DONE) && (Int_Occured == 0x00));
	if (PECI_Interrupt == 0) LogPass("Pass: Did not get an interrupt from PECI transaction when interrupt disabled..\n");
	else LogError("Error: Got an interrupt (%d) from PECI when interrupts disabled\n", PECI_Interrupt);
	
	WRITE_REG(PECI_CTL_STS, DONE);											// Clear DONE bit
	gicd_disable_int(PECI_INT);
}