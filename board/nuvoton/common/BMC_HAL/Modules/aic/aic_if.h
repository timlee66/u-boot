/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   aic_if.h                                                                                              */
/*            This file contains AIC driver interface                                                      */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _AIC_IF_H
#define _AIC_IF_H

#include "../../Common/hal_error.h"
#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler function prototype                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef HAL_STATUS (*AIC_Isr_T)(UINT32 param);

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Group related defines                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define AIC_GROUP_INTERRUPT_NONE            0xFF


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_initialize                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs AIC initialization                                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_Initialize (void);


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
HAL_STATUS AIC_RegisterHandler(UINT32 int_num, AIC_Isr_T func, UINT32 param);


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
HAL_STATUS AIC_EnableInt(UINT32 int_num);


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
HAL_STATUS AIC_DisableInt(UINT32 int_num);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_EnableGroupInt                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  group_bit_num - Number of the bit in the AIC GROUP Enable register                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable given interrupt in group.                                          */
/*                  if the function receives AIC_GROUP_INTERRUPT_NONE it exists without action             */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_EnableGroupInt(UINT32 group_bit_num);


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
/*                  if the function receives AIC_GROUP_INTERRUPT_NONE it exists without action             */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS AIC_DisableGroupInt(UINT32 group_bit_num);


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
BOOLEAN AIC_IsGroupIntEnabled(UINT32 group_bit_num);


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
HAL_STATUS AIC_EndOfService(UINT32 int_num);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        AIC_Isr                                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is the main IRQ handler                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void AIC_Isr(void);



#endif /*_AIC_IF_H */
