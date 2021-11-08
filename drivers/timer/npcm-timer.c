/*
 * NUVOTON npcm timer driver
 *
 * Copyright (C) 2017, NUVOTON, Incorporated
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <div64.h>
#include <dm.h>
#include <errno.h>
#include <timer.h>
#include <asm/io.h>
#include <clk.h>
#include <asm/arch/timer.h>

DECLARE_GLOBAL_DATA_PTR;

#define NPCM_CLOCK_RATE				1000000
#define HW_TIMER_INIT_VAL			0xFFFFFF

struct npcm_timer_priv {
	struct npcm_gptimer_regs *regs;
};

static struct npcm_timer_priv *timer_priv;

/* get HW timer data */
static u32 get_timer_data(void)
{
	u32 count;
	struct npcm_timer_priv *priv = timer_priv;

	if (!priv)
		return 0;

	count = readl(&priv->regs->tdr0) & 0x00ffffff;

	return count;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	unsigned long last, now, diff = 0;

	if (!gd->timer) {
		int i, ret;
		ret = dm_timer_init();
		if (ret) {
			for (i = 0 ; i < usec; i++);
			return;
		}
	}
	last = get_timer_data();

	while (diff < usec) {
		now = get_timer_data();
		if (now <= last)
			diff += (last - now);
		else
			diff += (HW_TIMER_INIT_VAL + 1 - now) + last;
		last = now;
	}
}

/* get timer count */
static u64 npcm_timer_get_count(struct udevice *dev)
{
	unsigned long now, diff;
	int ret;

	if (!gd->timer) {
		ret = dm_timer_init();
		if (ret)
			return ret;
	}
	now = get_timer_data();

	if (now <= gd->arch.lastinc)
		diff = gd->arch.lastinc - now;
	else
		diff = (HW_TIMER_INIT_VAL + 1 - now) + gd->arch.lastinc;

	gd->arch.timer_reset_value += diff;
	gd->arch.lastinc = now;

	return gd->arch.timer_reset_value;
}

static int timer_clock_init(struct udevice *dev)
{
	struct timer_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	struct clk clk;
	uint clkd[2]; /* clk_id and clk_no, timer clk_no is 1. */
	int ret;

	ret = fdtdec_get_int_array(gd->fdt_blob, dev_of_offset(dev),
			"clocks", clkd, 2);
	if (ret)
		return ret;

	ret = clk_get_by_index(dev, 0, &clk);
	if (ret < 0) {
		printf("Cannot find clk driver\n");
		return ret;
	}

	clk.id = clkd[1];

	/* To set timer clock source and divider */
	ret = clk_set_rate(&clk, uc_priv->clock_rate);
	clk_free(&clk);
	if (ret < 0)
		return ret;

	return 0;
}

static int npcm_timer_probe(struct udevice *dev)
{
	struct timer_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	struct npcm_timer_priv *priv = dev_get_priv(dev);
	int ret;

	timer_priv = priv;
	uc_priv->clock_rate = NPCM_CLOCK_RATE;

	ret = timer_clock_init(dev);
	if (ret)
		return ret;

	/* clear tcsr */
	writel(0, &priv->regs->tcsr0);
	/* set timer initial value */
	writel(HW_TIMER_INIT_VAL, &priv->regs->ticr0);

	/* configure timer and start */
	/* periodic mode
	 * input clock freq = 25Mhz
	 * prescale = 25
	 * clock rate = 25Mhz/25 = 1Mhz
	 */
	writel(TCSR_EN | TCSR_MODE_PERIODIC | TCSR_PRESCALE_25, &priv->regs->tcsr0);

	gd->arch.timer_reset_value = 0;
	gd->arch.lastinc = get_timer_data();
	gd->arch.tbl = 0;

	return 0;
}

static int npcm_timer_ofdata_to_platdata(struct udevice *dev)
{
	struct npcm_timer_priv *priv = dev_get_priv(dev);

	priv->regs = map_physmem((phys_addr_t)dev_read_addr_ptr(dev),
				 sizeof(struct npcm_gptimer_regs), MAP_NOCACHE);

	return 0;
}


static const struct timer_ops npcm_timer_ops = {
	.get_count = npcm_timer_get_count,
};

static const struct udevice_id npcm_timer_ids[] = {
	{ .compatible = "nuvoton,npcm845-timer" },
	{}
};

U_BOOT_DRIVER(npcm_timer) = {
	.name	= "npcm_timer",
	.id	= UCLASS_TIMER,
	.of_match = npcm_timer_ids,
	.of_to_plat = npcm_timer_ofdata_to_platdata,
	.priv_auto = sizeof(struct npcm_timer_priv),
	.probe = npcm_timer_probe,
	.ops	= &npcm_timer_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
