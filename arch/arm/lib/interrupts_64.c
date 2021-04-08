// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2013
 * David Feng <fenghua@phytium.com.cn>
 */

#include <common.h>
#include <linux/compiler.h>
#include <efi_loader.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_TARGET_ARBEL
extern void npcm_reginfo(void);
#endif

#ifdef CONFIG_NPCM850_DBG_INTERRUPTS
#include "../cpu/armv8/armv8_interrupts/psw.h"
#include "../cpu/armv8/armv8_interrupts/gic_v2.h"

#ifdef CONFIG_NPCM850_DBG_TIMER
extern void timer_handler(void);
#endif


#define INT_STATUS_ECC      (0xF0824000 + 0x228)
#define INT_ACK_ECC         (0xF0824000 + 0x244)
#define INT_STATUS_USERIF   (0xF0824000 + 0x145c)
#define INT_ACK_USERIF      (0xF0824000 + 0x1460)

void irq_handle(void)
{
	psw_t psw;
	irq_no irq;
	int rc;
	u32 int_status_ecc = 0;
	u32 int_status_userif = 0;
	
	psw_disable_and_save_interrupt(&psw);
	rc = gic_v2_find_pending_irq(&irq);
	if ( rc != IRQ_FOUND )  {
		goto restore_irq_out;
	}
	
    if ((irq != TIMER_IRQ) && (irq != MC_IRQ))
		gicd_clear_pending(irq);
		
	gicd_disable_int(irq);			/* Mask this irq */
	
#ifdef CONFIG_NPCM850_DBG_TIMER
	if(gicd_probe_pending(TIMER_IRQ))
	{
		timer_handler();
		gicd_clear_pending(TIMER_IRQ);		
		gicd_enable_int(TIMER_IRQ);			/* unmask this irq line */
	}
#endif

	if(gicd_probe_pending(MC_IRQ))
	{
		gicd_clear_pending(MC_IRQ);		
		int_status_ecc = __raw_readl(INT_STATUS_ECC);
		int_status_userif = __raw_readl(INT_STATUS_USERIF);
		__raw_writel(int_status_ecc , INT_ACK_ECC);		
		__raw_writel(int_status_userif , INT_ACK_USERIF);

		puts("\nMC_IRQ - Generated.\n");
				
		if (int_status_ecc & BIT(0))
			puts("\nint_status_ecc_bit0: A correctable ECC event has been detected.\n");

		if (int_status_ecc & BIT(1))
			puts("\nint_status_ecc_bit1: Multiple correctable ECC events have been detected.\n");

		if (int_status_ecc & BIT(2))
			puts("\nint_status_ecc_bit2: A uncorrectable ECC event has been detected.\n");

		if (int_status_ecc & BIT(3))
			puts("\nint_status_ecc_bit3: Multiple uncorrectable ECC events have been detected.\n");

		if (int_status_ecc & BIT(6))
			puts("\nint_status_ecc_bit6: One or more ECC writeback commands could not be executed.\n");

		if (int_status_ecc & BIT(7))
			puts("\nint_status_ecc_bit7: The scrub operation triggered by setting param ecc scrub start has completed.\n");
		
		if (int_status_ecc & BIT(8))
			puts("\nint_status_ecc_bit8: An ECC correctable error has been detected in a scrubbing read operation.\n");

		if (int_status_userif & BIT(0))
			puts("\nint_status_userif_bit0: A memory access outside the defined PHYSICAL memory space has occurred.\n");

		if (int_status_userif & BIT(1))
			puts("\nint_status_userif_bit1: Multiple accesses outside the defined PHYSICAL memory space have occurred.\n");
			
		puts("\nSet Acknowledge bits of int_ack_ecc int_ack_userif to clear errors.\n");
	
		gicd_clear_pending(MC_IRQ);			    /* Send EOI for this irq line */			
	    gicd_enable_int(MC_IRQ);			    /* unmask this mc irq line */
	}
        


restore_irq_out:
	psw_restore_interrupt(&psw);
}
#endif
int interrupt_init(void)
{
	return 0;
}

void enable_interrupts(void)
{
	return;
}

int disable_interrupts(void)
{
	return 0;
}

void show_regs(struct pt_regs *regs)
{
	int i;

	if (gd->flags & GD_FLG_RELOC)
		printf("elr: %016lx lr : %016lx (reloc)\n",
		       regs->elr - gd->reloc_off,
		       regs->regs[30] - gd->reloc_off);
	printf("elr: %016lx lr : %016lx\n", regs->elr, regs->regs[30]);

	for (i = 0; i < 29; i += 2)
		printf("x%-2d: %016lx x%-2d: %016lx\n",
		       i, regs->regs[i], i+1, regs->regs[i+1]);
	printf("\n");
#ifdef CONFIG_TARGET_ARBEL
	npcm_reginfo();
	while(1);              /* Debug- Now you can stop on debuger and look for fail registers values */
#endif
}

/*
 * do_bad_sync handles the impossible case in the Synchronous Abort vector.
 */
void do_bad_sync(struct pt_regs *pt_regs, unsigned int esr)
{
	efi_restore_gd();
	printf("Bad mode in \"Synchronous Abort\" handler, esr 0x%08x\n", esr);
	show_regs(pt_regs);
	panic("Resetting CPU ...\n");
}

/*
 * do_bad_irq handles the impossible case in the Irq vector.
 */
void do_bad_irq(struct pt_regs *pt_regs, unsigned int esr)
{
	efi_restore_gd();
	printf("Bad mode in \"Irq\" handler, esr 0x%08x\n", esr);
	show_regs(pt_regs);
	panic("Resetting CPU ...\n");
}

/*
 * do_bad_fiq handles the impossible case in the Fiq vector.
 */
void do_bad_fiq(struct pt_regs *pt_regs, unsigned int esr)
{
	efi_restore_gd();
	printf("Bad mode in \"Fiq\" handler, esr 0x%08x\n", esr);
	show_regs(pt_regs);
	panic("Resetting CPU ...\n");
}

/*
 * do_bad_error handles the impossible case in the Error vector.
 */
void do_bad_error(struct pt_regs *pt_regs, unsigned int esr)
{
	efi_restore_gd();
	printf("Bad mode in \"Error\" handler, esr 0x%08x\n", esr);
	show_regs(pt_regs);
	panic("Resetting CPU ...\n");
}

/*
 * do_sync handles the Synchronous Abort exception.
 */
void do_sync(struct pt_regs *pt_regs, unsigned int esr)
{
	efi_restore_gd();
	printf("\"Synchronous Abort\" handler, esr 0x%08x\n", esr);
	show_regs(pt_regs);
	panic("Resetting CPU ...\n");
}

/*
 * do_irq handles the Irq exception.
 */
void do_irq(struct pt_regs *pt_regs, unsigned int esr)
{
#ifdef CONFIG_NPCM850_DBG_INTERRUPTS
	irq_handle();
#else
	efi_restore_gd();
	printf("\"Irq\" handler, esr 0x%08x\n", esr);
	show_regs(pt_regs);
	panic("Resetting CPU ...\n");
#endif
}

/*
 * do_fiq handles the Fiq exception.
 */
void do_fiq(struct pt_regs *pt_regs, unsigned int esr)
{
	efi_restore_gd();
	printf("\"Fiq\" handler, esr 0x%08x\n", esr);
	show_regs(pt_regs);
	panic("Resetting CPU ...\n");
}

/*
 * do_error handles the Error exception.
 * Errors are more likely to be processor specific,
 * it is defined with weak attribute and can be redefined
 * in processor specific code.
 */
void __weak do_error(struct pt_regs *pt_regs, unsigned int esr)
{
	efi_restore_gd();
	printf("\"Error\" handler, esr 0x%08x\n", esr);
	show_regs(pt_regs);
	panic("Resetting CPU ...\n");
}
