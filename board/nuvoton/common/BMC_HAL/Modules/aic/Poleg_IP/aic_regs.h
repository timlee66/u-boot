/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   aic_regs.h                                                                                            */
/*            This file contains definitions of AIC registers                                              */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _AIC_REGS_H
#define _AIC_REGS_H

#include "../../../Chips/chip.h"


#define GICC_CTLR              (AIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET + 0x0000), AIC_ACCESS, 32
#define GICC_PMR               (AIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET + 0x0004), AIC_ACCESS, 32
#define GICC_IAR               (AIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET + 0x000C), AIC_ACCESS, 32
#define GICC_EOIR              (AIC_BASE_ADDR + GIC_CPU_INTERFACE_OFFSET + 0x0010), AIC_ACCESS, 32


#define GICD_CTLR              (AIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0000), AIC_ACCESS, 32
#define GICD_TYPER             (AIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0004), AIC_ACCESS, 32
#define GICD_IIDR              (AIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0008), AIC_ACCESS, 32
#define GICD_ISENABLER         (AIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0100), AIC_ACCESS, 32
#define GICD_ICENABLER         (AIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0180), AIC_ACCESS, 32
#define GICD_IPRIORITYR        (AIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0400), AIC_ACCESS, 32
#define GICD_ITARGETSR         (AIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0800), AIC_ACCESS, 32
#define GICD_ICFGR             (AIC_BASE_ADDR + GIC_DISTRIBUTOR_OFFSET + 0x0C00), AIC_ACCESS, 32

#endif /* _AIC_REGS_H */
