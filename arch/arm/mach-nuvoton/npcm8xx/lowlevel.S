/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2014-2015 Freescale Semiconductor
 *
 * Extracted from armv8/start.S
 */

#include <config.h>
#include <linux/linkage.h>
#include <asm/gic.h>
#include <asm/macro.h>
#ifdef CONFIG_MP
#include "mp.h"
#endif
#include <asm/u-boot.h>

/* Get GIC offset
* For LS1043a rev1.0, GIC base address align with 4k.
* For LS1043a rev1.1, if DCFG_GIC400_ALIGN[GIC_ADDR_BIT]
* is set, GIC base address align with 4K, or else align
* with 64k.
* output:
*	x0: the base address of GICD
*	x1: the base address of GICC
*/
ENTRY(get_gic_offset)
	ldr     x0, =GICD_BASE
#ifdef CONFIG_GICV2
	ldr     x1, =GICC_BASE
#endif
	ret
ENDPROC(get_gic_offset)

ENTRY(smp_kick_all_cpus)
	/* Kick secondary cpus up by SGI 0 interrupt */
#if defined(CONFIG_GICV2) || defined(CONFIG_GICV3)
	mov	x29, lr			/* Save LR */
	bl	get_gic_offset
	bl	gic_kick_secondary_cpus
	mov	lr, x29			/* Restore LR */
#endif
	ret
ENDPROC(smp_kick_all_cpus)


ENTRY(lowlevel_init)
	mov	x29, lr			/* Save LR */

	/* unmask SError and abort */
	msr daifclr, #4

	/* Set HCR_EL2[AMO] so SError @EL2 is taken */
	mrs	x0, hcr_el2
	orr	x0, x0, #0x20			/* AMO */
	msr	hcr_el2, x0
	isb

	switch_el x1, 1f, 100f, 100f	/* skip if not in EL3 */
1:


	/* Initialize GIC Secure Bank Status */
#if defined(CONFIG_GICV2) || defined(CONFIG_GICV3)
	branch_if_slave x0, 1f
	bl	get_gic_offset
	bl	gic_init_secure
1:
#ifdef CONFIG_GICV3
	ldr	x0, =GICR_BASE
	bl	gic_init_secure_percpu
#elif defined(CONFIG_GICV2)
	bl	get_gic_offset
	bl	gic_init_secure_percpu
#endif
#endif

100:
	branch_if_master x0, x1, 2f

#if defined(CONFIG_MP) && defined(CONFIG_ARMV8_MULTIENTRY)
	ldr	x0, =secondary_boot_func
	blr	x0
#endif

2:
	switch_el x1, 1f, 100f, 100f	/* skip if not in EL3 */
1:
100:
1:

	mov	lr, x29			/* Restore LR */
	ret
ENDPROC(lowlevel_init)

#ifdef CONFIG_MP
	/* Keep literals not used by the secondary boot code outside it */
	.ltorg

	/* Using 64 bit alignment since the spin table is accessed as data */
	.align 4
	.global secondary_boot_code
	/* Secondary Boot Code starts here */
secondary_boot_code:
	.global __spin_table
__spin_table:
	.space CONFIG_MAX_CPUS*SPIN_TABLE_ELEM_SIZE

	.align 2
ENTRY(secondary_boot_func)
	/*
	 * MPIDR_EL1 Fields:
	 * MPIDR[1:0] = AFF0_CPUID <- Core ID (0,1)
	 * MPIDR[7:2] = AFF0_RES
	 * MPIDR[15:8] = AFF1_CLUSTERID <- Cluster ID (0,1,2,3)
	 * MPIDR[23:16] = AFF2_CLUSTERID
	 * MPIDR[24] = MT
	 * MPIDR[29:25] = RES0
	 * MPIDR[30] = U
	 * MPIDR[31] = ME
	 * MPIDR[39:32] = AFF3
	 *
	 * Linear Processor ID (LPID) calculation from MPIDR_EL1:
	 * (We only use AFF0_CPUID and AFF1_CLUSTERID for now
	 * until AFF2_CLUSTERID and AFF3 have non-zero values)
	 *
	 * LPID = MPIDR[15:8] | MPIDR[1:0]
	 */
	mrs	x0, mpidr_el1
	ubfm	x1, x0, #8, #15
	ubfm	x2, x0, #0, #1
	orr	x10, x2, x1, lsl #2	/* x10 has LPID */
	ubfm    x9, x0, #0, #15         /* x9 contains MPIDR[15:0] */
	/*
	 * offset of the spin table element for this core from start of spin
	 * table (each elem is padded to 64 bytes)
	 */
	lsl	x1, x10, #6
	ldr	x0, =__spin_table
	/* physical address of this cpus spin table element */
	add	x11, x1, x0

	ldr	x0, =__real_cntfrq
	ldr	x0, [x0]
	msr	cntfrq_el0, x0	/* set with real frequency */
	str	x9, [x11, #16]	/* LPID */
	mov	x4, #1
	str	x4, [x11, #8]	/* STATUS */
	dsb	sy
#if defined(CONFIG_GICV3)
	gic_wait_for_interrupt_m x0
#elif defined(CONFIG_GICV2)
	bl	get_gic_offset
	mov	x0, x1
        gic_wait_for_interrupt_m x0, w1
#endif

slave_cpu:
	wfe
	ldr	x0, [x11]
	cbz	x0, slave_cpu
#ifndef CONFIG_ARMV8_SWITCH_TO_EL1
	mrs     x1, sctlr_el2
#else
	mrs     x1, sctlr_el1
#endif
	tbz     x1, #25, cpu_is_le
	rev     x0, x0                  /* BE to LE conversion */
cpu_is_le:
	ldr	x5, [x11, #24]
	cbz	x5, 1f

#ifdef CONFIG_ARMV8_SWITCH_TO_EL1
	adr	x4, secondary_switch_to_el1
	ldr	x5, =ES_TO_AARCH64
#else
	ldr	x4, [x11]
	ldr	x5, =ES_TO_AARCH32
#endif
	bl	secondary_switch_to_el2

1:
#ifdef CONFIG_ARMV8_SWITCH_TO_EL1
	adr	x4, secondary_switch_to_el1
#else
	ldr	x4, [x11]
#endif
	ldr	x5, =ES_TO_AARCH64
	bl	secondary_switch_to_el2

ENDPROC(secondary_boot_func)

ENTRY(secondary_switch_to_el2)
	switch_el x6, 1f, 0f, 0f
0:	ret
1:	armv8_switch_to_el2_m x4, x5, x6
ENDPROC(secondary_switch_to_el2)

ENTRY(secondary_switch_to_el1)
	mrs	x0, mpidr_el1
	ubfm	x1, x0, #8, #15
	ubfm	x2, x0, #0, #1
	orr	x10, x2, x1, lsl #2	/* x10 has LPID */

	lsl	x1, x10, #6
	ldr	x0, =__spin_table
	/* physical address of this cpus spin table element */
	add	x11, x1, x0

	ldr	x4, [x11]

	ldr	x5, [x11, #24]
	cbz	x5, 2f

	ldr	x5, =ES_TO_AARCH32
	bl	switch_to_el1

2:	ldr	x5, =ES_TO_AARCH64

switch_to_el1:
	switch_el x6, 0f, 1f, 0f
0:	ret
1:	armv8_switch_to_el1_m x4, x5, x6
ENDPROC(secondary_switch_to_el1)

	/* Ensure that the literals used by the secondary boot code are
	 * assembled within it (this is required so that we can protect
	 * this area with a single memreserve region
	 */
	.ltorg

	/* 64 bit alignment for elements accessed as data */
	.align 4
	.global __real_cntfrq
__real_cntfrq:
	.quad COUNTER_FREQUENCY
	.globl __secondary_boot_code_size
	.type __secondary_boot_code_size, %object
	/* Secondary Boot Code ends here */
__secondary_boot_code_size:
	.quad .-secondary_boot_code
#endif
