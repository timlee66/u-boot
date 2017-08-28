/*
	* (C) Copyright 2000
	* Wolfgang Denk, DENX Software Engineering, wd@denx.de.
	*
	* SPDX-License-Identifier:	GPL-2.0+
	*/

/*
	* Cache support: switch on or off, get status
	*/
#include <common.h>
#include <command.h>
#include <linux/compiler.h>



int do_usb_device(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint32_t device_number = 0;
	uint32_t *pAppAddr __attribute__((unused));

	if(argc > 1)
	{
		device_number = (argv[1][0]) - '0';
		if(device_number > 9)
		{
                printf( " Invalid device number\n" );
				return 0;
		}
	}

	printf( " running device %d \n",  device_number);

	*((volatile uint32_t *)(0x7000)) = 0xa5b6c7d8;
	*((volatile uint32_t *)(0x7004)) = device_number;

	pAppAddr= (uint32_t *)(0x8000);     /* load bin address */

	/* jump to application */
	asm volatile(	"ldr r0,%[addr] \n"
					"bx r0 \n"
					: :[addr]"m"(pAppAddr):"memory");

	return 0;
}


U_BOOT_CMD(
	usb_device,   2,   1,     do_usb_device,
	"usb device test",
	"<device_num>"
);
