/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   aic_drv.c                                                                                             */
/*            This file contains implementation of AIC module                                              */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#include "aic_drv.h"
#include "aic_regs.h"

#include "../../../Chips/chip_if.h"


#define GIC_PRI_LOWEST     0xf0
#define GIC_PRI_IRQ        0xa0
#define GIC_PRI_IPI        0x90 /* IPIs must preempt normal interrupts */
#define GIC_PRI_HIGHEST    0x80 /* Higher priorities belong to Secure-World */

#define GICC_CTL_EOI (0x1 << 9)
#define GICD_CTL_ENABLE 0x1
#define GICC_CTL_ENABLE 0x1

#define GICD_TYPE_LINES 0x01f
#define GICD_TYPE_CPUS  0x0e0
#define GICD_TYPE_SEC   0x400
/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Handler Table                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

typedef struct
{
	AIC_Isr_T   func;
	UINT32      param;
}   AIC_IsrEntry_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Function forward declaration                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
static AIC_IsrEntry_T AIC_handler_table[GIC_INTERRUPT_NUM];


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_Initialize                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs AIC initialization                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_Initialize (void)
{
	UINT i;
	volatile UINT32 * reg_address;
	UINT32 cpumask;
	/*UINT32 gic_type; */
	/*UINT32 gic_iidr; */
	/*UINT32 gic_lines; */
	/*UINT32 gic_cpus; */

	/*-----------------------------------------------------------------------------------------------------*/
	/* clean interrupt handler function pointers table                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	for(i=0; i<GIC_INTERRUPT_NUM; i++)
	{
        AIC_handler_table[i].func = NULL;
        AIC_handler_table[i].param = 0;
	}

	cpumask = REG_READ(GICD_ITARGETSR) & 0xff;
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	REG_WRITE(GICD_CTLR, (UINT32)0x00);    /* Disable GIC Distributor */




#ifndef __LINUX_KERNEL_ONLY__
/*    gic_type = REG_READ(GICD_TYPER); */
/*    gic_iidr = REG_READ(GICD_IIDR); */
/*    gic_lines = 32 * ((gic_type & GICD_TYPE_LINES) + 1); */
/*    gic_cpus = 1 + ((gic_type & GICD_TYPE_CPUS) >> 5); */

/*    No Console at this point. */
/*    printf("GIC: %d lines, %d cpu%s%s (IID %8.8x).\n", */
/*           gic_lines, gic_cpus, (gic_cpus == 1) ? "" : "s", */
/*           (gic_type & GICD_TYPE_SEC) ? ", secure" : "", */
/*           gic_iidr); */
#endif


	/*
     * Set all global interrupts to be level triggered, active High.
     */
	for (i = 32; i < GIC_INTERRUPT_NUM; i += 16)
	{
	    reg_address = (volatile UINT32 *)(REG_ADDR(GICD_ICFGR) + ((i/16) * 4));
        *reg_address = (UINT32)0x00;
	}
	/*
     * Set all global interrupts to this CPU only (CPU0).
     */
	for (i = 32; i < GIC_INTERRUPT_NUM; i += 4)
	{
	    reg_address = (volatile UINT32 *)(REG_ADDR(GICD_ITARGETSR) + ((i/4) * 4));
        *reg_address = cpumask;
	}

	/*
     * Set priority on all interrupts.
     */
	for (i = 32; i < GIC_INTERRUPT_NUM; i += 4)
	{
	    reg_address = (volatile UINT32 *)(REG_ADDR(GICD_IPRIORITYR) + ((i/4) * 4));
        *reg_address =  (GIC_PRI_IRQ<<24) | (GIC_PRI_IRQ<<16) | (GIC_PRI_IRQ<<8) | (GIC_PRI_IRQ);
	}

	/*
     * Disable all interrupts.
     */
	for (i = 32; i < GIC_INTERRUPT_NUM; i += 32)
	{
	    reg_address = (volatile UINT32 *)(REG_ADDR(GICD_ICENABLER) + ((i/32) * 4));
        *reg_address = 0xFFFFFFFF;
	}


	REG_WRITE(GICD_CTLR, (UINT32)GICD_CTL_ENABLE);                   /* Enable GIC Distributor */

	REG_WRITE(GICD_ICENABLER, 0xFFFFFFFF);                           /* Disable all SGI + PPI */

	REG_WRITE(GICC_PMR, (UINT32)0xFF);                               /* Set Priority Mask */

	REG_WRITE(GICC_CTLR, (UINT32)GICC_CTL_ENABLE | GICC_CTL_EOI);    /* Enable GIC CPU Interface */

	return HAL_OK;
}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_RegisterHandler                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  func -                                                                                 */
/*                  int_num -                                                                              */
/*                  param -                                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs interrup registration                                            */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_RegisterHandler(UINT32 int_num, AIC_Isr_T func, UINT32 param)
{
	if((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	AIC_handler_table[int_num].func = func;
	AIC_handler_table[int_num].param = param;

	return HAL_OK;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_EnableInt                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable given interrupt                                                    */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_EnableInt(UINT32 int_num)
{
	volatile UINT32 * reg_address;
	UINT32 mask = 1 << (int_num % 32);

	if ((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
	{
        return HAL_ERROR_BAD_DEVNUM;
	}


	reg_address = (volatile UINT32 *)(REG_ADDR(GICD_ISENABLER) + ((int_num/32) * 4));
	*reg_address = mask;
	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_DisableInt                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disable given interrupt                                                   */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_DisableInt(UINT32 int_num)
{
	volatile UINT32 * reg_address;
	UINT32 mask = 1 << (int_num % 32);

	if ((int_num >= GIC_INTERRUPT_NUM) || (int_num < 32))
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	reg_address = (volatile UINT32 *)(REG_ADDR(GICD_ICENABLER) + ((int_num/32) * 4));
	*reg_address = mask;
	return HAL_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_EnableGroupInt                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  group_bit_num - Number of the bit in the AIC GROUP Enable register                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable given interrupt in group                                           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_EnableGroupInt(UINT32 group_bit_num)
{
	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_DisableGroupInt                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  group_bit_num - Number of the bit in the AIC GROUP Enable register                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine disable given interrupt in group                                          */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_DisableGroupInt(UINT32 group_bit_num)
{

	return HAL_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_IsGroupIntEnabled                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  group_bit_num - Number of the bit in the AIC GROUP Enable register                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns true is group_bit_num is set                                      */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN AIC_IsGroupIntEnabled(UINT32 group_bit_num)
{
	return 1;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_EndOfService                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  int_num -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine acknowledges that IRQ is served                                           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_EndOfService(UINT32 int_num)
{
	REG_WRITE(GICC_EOIR, int_num);
	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_Isr                                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is the main IRQ handler                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void AIC_Isr(void)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Reading the highest priority unhandled interrupt                                                    */
	/* IPER returns the interrupt number multipled by 4 (for easier table lookup)                          */
	/*-----------------------------------------------------------------------------------------------------*/
	UINT32 irq_num;


	while (1)
	{
        irq_num = REG_READ(GICC_IAR);
		irq_num &= (UINT32)0x03FF;


		if ((irq_num >= 32) && (irq_num < GIC_INTERRUPT_NUM))
		{
           /*-------------------------------------------------------------------------------------------------*/
           /* If handler installed, execute it                                                                */
           /*-------------------------------------------------------------------------------------------------*/
           if (AIC_handler_table[irq_num].func)
           {
               AIC_handler_table[irq_num].func(AIC_handler_table[irq_num].param);
           }


           AIC_EndOfService(irq_num);
		   continue;
		}
		break;
	}

}



