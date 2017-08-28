/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   PolegSVB_low_level_init.S                                                                            */
/*            This file contains NPCM750 low level init                                                    */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

#include <asm-offsets.h>
#include <config.h>
#include <version.h>
#include <linux/linkage.h>

ENTRY(board_lowlevel_init)

	/*
	 * Setup a temporary stack
	 */
	ldr	sp, =CONFIG_SYS_INIT_SP_ADDR
	bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */
	sub	sp, #GD_SIZE
	bic	sp, sp, #7
	mov	r9, sp

	/*
	 * Save the old lr(passed in ip) and the current lr to stack
	 */
	push	{ip, lr}

	/*
	 *  Invalidate l2-Cache
	 */
	bl	v7_outer_cache_inval_all

	pop	{ip, pc}

ENDPROC(board_lowlevel_init)


