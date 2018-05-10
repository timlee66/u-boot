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

static UART_DEV_T uart_dev = UART3_DEV;

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
	/* Get current settings that BB had previously set */
	uart_dev = GCR_Core_Uart_Get();

	return 0;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        nuvoton_serial_set_console_env                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  c - char to write to UART                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write single char to UART                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void nuvoton_serial_set_console_env(void)
{
	char serial_base[32];

	sprintf(serial_base, "uart8250,mmio32,0x%lx", (long int)UART_PHYS_BASE_ADDR(uart_dev));
	setenv("earlycon", serial_base);

	if(uart_dev == UART3_DEV)
	{
		setenv("console", "ttyS3,115200n8");
		setenv("bootargs", "earlycon=uart8250,mmio32,0xf0004000 root=/dev/ram console=ttyS3,115200n8 mem=464M ramdisk_size=48000 basemac=${ethaddr}");
	}
	else //  (uart_dev == UART0_DEV)
	{
		setenv("console", "ttyS0,115200n8");
		setenv("bootargs", "earlycon=uart8250,mmio32,0xf0001000 root=/dev/ram console=ttyS0,115200n8 mem=464M ramdisk_size=48000 basemac=${ethaddr}");

	}
	return;
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
	UART_PutC(uart_dev, c);

	if (c == '\n')
	{
        UART_PutC(uart_dev, '\r');
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
	return UART_GetC(uart_dev);
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
	return UART_TestRX(uart_dev);
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
