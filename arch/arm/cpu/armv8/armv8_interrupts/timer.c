#include <stdio.h>
#include <stdlib.h>
#include <asm/io.h>
#include <asm/system.h>
#include "aarch64.h"
#include "gic_v2.h"
#include "timer.h"


/* System Counter */
struct sctr_regs {
	u32 cntcr;
	u32 cntsr;
	u32 cntcv1;
	u32 cntcv2;
	u32 resv1[4];
	u32 cntfid0;
	u32 resv2[1003];
	u32 counterid[1];
};

#define SC_CNTCR_ENABLE		(1 << 0)
#define SC_CNTCR_HDBG		(1 << 1)



#ifdef CONFIG_TARGET_ARBEL_PALLADIUM
#define TIMER_WAIT	1 	/* Assert Timer IRQ after n secs */
#else
#define TIMER_WAIT	60 	/* Assert Timer IRQ after n secs */
#endif

#define SCTR_BASE_ADDR   0xF07FC000

static uint64_t cntfrq;		/* System frequency */

int sys_counter_init(void)
{
	struct sctr_regs *sctr = (struct sctr_regs *)SCTR_BASE_ADDR;
	unsigned long val;

	cntfrq = raw_read_cntfrq_el0();
	writel(cntfrq, &sctr->cntfid0); 

	/* Enable system counter */
	val = readl(&sctr->cntcr);
	val |= SC_CNTCR_HDBG | SC_CNTCR_ENABLE;
	writel(val, &sctr->cntcr);

	return 0;
}

/* Assert Timer IRQ after n secs */
void timer_handler(void)
{
	uint64_t ticks, current_cnt;
	volatile uint32_t val;


	// Disable the timer
	disable_cntv();
	val = raw_read_cntv_ctl();
	debug(" raw_read_cntv_ctl = 0x%x \n", val);
	gicd_clear_pending(TIMER_IRQ);
    val = raw_read_cntv_off();
	debug(" raw_read_cntv_off = 0x%x \n", val);

	// Next timer IRQ is after n sec.
	ticks = (TIMER_WAIT * cntfrq);
	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();
	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);
	
	val = raw_read_cntv_cval_el0();
	printf(" Timer handler isr (INT-27) -> raw_read_cntv_cval_el0 = 0x%x \n", val);

	// Enable the timer
	enable_cntv();
	val = raw_read_cntv_ctl();
	debug(" raw_read_cntv_ctl = 0x%x \n", val);
}


void timer_test(void)
{
	volatile uint32_t val;
	uint64_t ticks, current_cnt;

	sys_counter_init();

	// GIC Init
	gic_v2_initialize(TIMER_IRQ , 0x1);

	val = raw_read_current_el();
	debug(" raw_read_current_el = 0x%x \n", val);
	//val = raw_read_rvbar_el1();

	val = raw_read_vbar_el1();
	debug(" raw_read_vbar_el1 = 0x%x \n", val);

	val = raw_read_daif();
	debug(" raw_read_daif = 0x%x \n", val);

	// Disable the timer
	disable_cntv();
	val = raw_read_cntv_ctl();
	debug(" raw_read_cntv_ctl = 0x%x \n", val);

	cntfrq = raw_read_cntfrq_el0();
	printf(" Timer Timeout %d sec's raw_read_cntfrq_el0 = %lld Hz \n", TIMER_WAIT, cntfrq);
	// Next timer IRQ is after n sec(s).
	ticks = (TIMER_WAIT * cntfrq);
	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();
	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);
	val = raw_read_cntv_cval_el0();
	debug(" raw_read_cntv_cval_el0 = 0x%x \n", val);

	// Enable the timer
	enable_cntv();
	val = raw_read_cntv_ctl();
	debug(" raw_read_cntv_ctl = 0x%x \n", val);

	// Enable IRQ 
	enable_irq();
	val = raw_read_daif();
	debug(" raw_read_daif = 0x%x \n", val);
}
