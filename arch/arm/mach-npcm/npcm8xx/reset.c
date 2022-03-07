// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/rst.h>
#include <asm/arch/gcr.h>
#include <asm/arch/cpu.h>

void reset_cpu(ulong ignored)
{
	/* Generate a watchdog0 reset */
	writel(WTCR_WTR | WTCR_WTRE | WTCR_WTE, WTCR0_REG);

	while (1)
		;
}

void reset_misc(void)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)npcm_get_base_gcr();

	/* clear WDC */
	writel(readl(&gcr->intcr2) & ~(1 << INTCR2_WDC), &gcr->intcr2);
}

int npcm_get_reset_status(void)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)npcm_get_base_gcr();
	u32 val;

	val = readl(&gcr->ressr);
	if (!val)
		val = readl(&gcr->intcr2);

	return val & RST_STS_MASK;
}
