/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nuvoton_serial.c                                                                                      */
/*            This file contains implementation of serial driver for U-Boot                                */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

#include <config.h>
#include <common.h>
#include <serial.h>

#undef MAX
#undef MIN

#include "BMC_HAL/Chips/chip_if.h"

#define UART_DEV UART3_DEV

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        serial_init                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs UART initialization                                              */
/*---------------------------------------------------------------------------------------------------------*/
int nuvoton_serial_init(void)
{
	/* in NPCM750 the BootBlock already configured the UART and the muxes were done according to GPIO and board type */
#ifndef NPCM750
	UART_Init(UART_DEV, UART_MUX_MODE3_HSP1_UART1__HSP2_UART2__UART3_SI2, CONFIG_BAUDRATE);
#endif
	return 0;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        serial_putc                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  c - char to write to UART                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write single char to UART                                                 */
/*---------------------------------------------------------------------------------------------------------*/

void nuvoton_serial_putc( const char c )
{
	UART_PutC(UART_DEV, c);

	if (c == '\n')
	{
        UART_PutC(UART_DEV, '\r');
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        serial_puts                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  s - string to write to UART                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes whole string to UART                                               */
/*---------------------------------------------------------------------------------------------------------*/
void nuvoton_serial_puts( const INT8 *s )
{
	while (*s)
	{
	serial_putc( *s++ );
	}
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        serial_getc                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads char from UART                                                      */
/*---------------------------------------------------------------------------------------------------------*/
int nuvoton_serial_getc( void )
{
	return UART_GetC(UART_DEV);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        serial_tstc                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine test if there is a char on UART                                           */
/*---------------------------------------------------------------------------------------------------------*/
int nuvoton_serial_tstc( void )
{
	return UART_TestRX(UART_DEV);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        serial_setbrg                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a stub                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void nuvoton_serial_setbrg( void )
{
}

static struct serial_device nuvoton_serial_drv = {
	.name	= "nuvoton_serial",
	.start	= nuvoton_serial_init,
	.stop	= NULL,
	.setbrg	= nuvoton_serial_setbrg,
	.putc	= nuvoton_serial_putc,
	.puts	= default_serial_puts,
	.getc	= nuvoton_serial_getc,
	.tstc	= nuvoton_serial_tstc,
};

void nuvoton_serial_initialize(void)
{
	serial_register(&nuvoton_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
	return &nuvoton_serial_drv;
}
