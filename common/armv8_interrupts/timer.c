#include <stdio.h>
#include <stdlib.h>
#include <asm/io.h>
#include "aarch64.h"
#include "gic_v2.h"
#include "timer.h"
#include "../../arch/arm/include/asm/system.h"

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



#define TIMER_DEBUG     0
#define TIMER_WAIT	10 	/* Assert Timer IRQ after n secs */

#define SCTR_BASE_ADDR   0xF07FC000

static uint64_t cntfrq;		/* System frequency */

int sys_counter_init(void)
{
	struct sctr_regs *sctr = (struct sctr_regs *)SCTR_BASE_ADDR;
	unsigned long val, freq;

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
	gicd_clear_pending(TIMER_IRQ);
        val = raw_read_cntv_off();
	// Next timer IRQ is after n sec.
	ticks = (TIMER_WAIT * cntfrq);
	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();
	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);
	
	val = raw_read_cntv_cval_el0();

	// Enable the timer
	enable_cntv();
	val = raw_read_cntv_ctl();
}


void timer_test(void)
{
	volatile uint32_t val;
	uint64_t ticks, current_cnt;

	sys_counter_init();

	// GIC Init
	gic_v2_initialize(TIMER_IRQ);

	val = raw_read_current_el();

	//val = raw_read_rvbar_el1();

	val = raw_read_vbar_el1();

	val = raw_read_daif();

	// Disable the timer
	disable_cntv();
	val = raw_read_cntv_ctl();
	cntfrq = raw_read_cntfrq_el0();

	// Next timer IRQ is after n sec(s).
	ticks = (TIMER_WAIT * cntfrq);
	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();
	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);
	val = raw_read_cntv_cval_el0();

	// Enable the timer
	enable_cntv();
	val = raw_read_cntv_ctl();

	// Enable IRQ 
	enable_irq();
	val = raw_read_daif();
#if 0
#if TIMER_DEBUG // Observe CNTP_CTL_EL0[2]: ISTATUS
	while(1){
		current_cnt = raw_read_cntvct_el0();
		val = raw_read_cntv_ctl();
		val = raw_read_spsr_el1();
		val = raw_read_isr_el1();
	}
#else
	while(1){
		wfi();	/* Wait for Interrupt */
	}
#endif
#endif
}
