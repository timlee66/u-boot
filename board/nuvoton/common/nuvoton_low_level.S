/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   HermonSVB_low_level_init.S                                                                            */
/*            This file contains nuvoton low level init                                                    */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/


#include <config.h>
#include <version.h>




.globl lowlevel_init
lowlevel_init:

/*
;----------------------------------------------------------
; Execute board dependent low level code
; Configuring Memory if needed
;----------------------------------------------------------
*/
#ifdef CONFIG_BOARD_LOWLEVEL_INIT
	mov 	r11, lr
	bl	board_lowlevel_init
	mov		lr, r11
#endif


#if 0    /* Trego - removed redundant code, already done in uboot. */
/*
;----------------------------------------------------------
; Disable Interrupt, This is for safe ...
;----------------------------------------------------------
*/
	MRS     r0, CPSR
	ORR     r0, r0, #0xC0
	MSR     CPSR_c, r0


/*
;----------------------------------------------------------
; Set mode to SVC, interrupts disabled (just paranoid)
;----------------------------------------------------------
*/
	MRS   r0, cpsr
	BIC   r0, r0, #0xF0000000
	BIC   r0, r0, #0x1F
	ORR   r0, r0, #0xD3
	MSR   cpsr_fc, r0


/*
;----------------------------------------------------------
; Enabling Instruction Cache
;----------------------------------------------------------
*/
	MRC     p15, 0, r0, c1, c0          /* get control register   */
	ORR     r0, r0, #0x00001000         /* I-cache                */
	MCR     p15, 0, r0, c1, c0          /* enable I cache         */
#endif
/*
;----------------------------------------------------------
; --- back to arch calling code
;----------------------------------------------------------
*/
	MOV     pc,     lr
