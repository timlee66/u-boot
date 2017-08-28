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
#include <usb.h>
#include "../../../drivers/usb/host/ehci.h"
#include <asm/io.h>

#undef MAX
#undef MIN

#include "BMC_HAL/Chips/chip_if.h"

#define USB_HOST_BA				(USBH_EHCI_BASE_ADDR+0x100)  /* changed by az */


#define  IPSRST2                        (CLK_BASE_ADDR + 0x24)
#define IPSRST2_USBHOST_BIT_POS  26

#define  IPSRST3                        (CLK_BASE_ADDR + 0x34)
#define IPSRST3_USBPHY1_BIT_POS  24
#define IPSRST3_USBPHY2_BIT_POS  25


#define  USBTEST                        (GCR_BASE_ADDR + 0x140)
#define USBTEST_USB1PHYCTL_BIT_POS  28

#define  USB2TEST                       (GCR_BASE_ADDR + 0x144)
#define USBTEST_USB2PHYCTL_BIT_POS  28

#define SET_BIT_IN_REG(reg,bit_pos)  writel(readl(reg) |   (1 <<  bit_pos) ,reg);
#define CLR_BIT_IN_REG(reg,bit_pos)  writel(readl(reg) & (~(1 <<  bit_pos)) ,reg);
/*
	* EHCI host controller init
	*/
int ehci_hcd_init(int index, enum usb_init_type init,
                struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
/* 	// loopback */
/* 	regVal=*((volatile uint32_t *)(REG_INTCR3)); */
/* 	*((volatile uint32_t *)(REG_INTCR3)) = regVal | 0x01000000; */

		/* reset host */
	SET_BIT_IN_REG(IPSRST2, IPSRST2_USBHOST_BIT_POS);
		mdelay(20);


		/* enable phy */
		CLR_BIT_IN_REG(IPSRST3, IPSRST3_USBPHY1_BIT_POS);
		CLR_BIT_IN_REG(IPSRST3, IPSRST3_USBPHY2_BIT_POS);
		mdelay(20);

		/* set RS (Reset Sequence) bit */
		SET_BIT_IN_REG(USBTEST, USBTEST_USB1PHYCTL_BIT_POS);
		SET_BIT_IN_REG(USB2TEST, USBTEST_USB2PHYCTL_BIT_POS);
		mdelay(20);

		/* enable host */
	CLR_BIT_IN_REG(IPSRST2, IPSRST2_USBHOST_BIT_POS);
		mdelay(20);


	/* az	if (utmi_init() < 0) */
/* az		return -1; */
	/* enable D-CACHE */
	/*unsigned int R0; */
	  asm volatile(	"MRC     p15, 0, R0, c1, c0, 0\n"  );
	  asm volatile(	"ORR     R0, R0, #(0x1  <<2)\n"  );
	  asm volatile(	"MCR     p15, 0, R0, c1, c0, 0\n"  );

	*hccr = (struct ehci_hccr *)(USB_HOST_BA);
	*hcor = (struct ehci_hcor *)((uint32_t) *hccr
			+ HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));

/*	debug("armada100-ehci: init hccr %x and hcor %x hc_length %d\n", */
/*		(uint32_t)*hccr, (uint32_t)*hcor, */
/*		(uint32_t)HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase))); */

	return 0;
}

/*az - usb */
int board_usb_init(int index, enum usb_init_type init)
{
	return 0;
}


/*
	* EHCI host controller stop
	*/
int ehci_hcd_stop(int index)
{
	return 0;
}
