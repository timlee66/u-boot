// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology.
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <timer.h>
#include <asm/io.h>

#define NPCM_TIMER_CLOCK_RATE			1000000UL
#define NPCM_TIMER_INPUT_RATE			25000000UL
#define NPCM_TIMER_INIT_VAL			0x00FFFFFF
#define NPCM_TIMER_TDR_MASK			GENMASK(23, 0)

/* Register offsets */
#define TCR0	0x0
#define TICR0	0x8
#define TDR0	0x10

/* TCR fields */
#define TCR_MODE_PERIODIC	BIT(27)
#define TCR_EN			BIT(30)
#define TCR_PRESCALE_25		(25 - 1)

struct npcm_timer_priv {
	void __iomem *base;
	u32 last_count;
	u64 counter;
};

static u64 npcm_timer_get_count(struct udevice *dev)
{
	struct npcm_timer_priv *priv = dev_get_priv(dev);
	u32 val;

	/* The counter is couting down, inverse the value */
	val = NPCM_TIMER_INIT_VAL - (readl(priv->base + TDR0) & NPCM_TIMER_TDR_MASK);

	if (val >= priv->last_count)
		priv->counter += (val - priv->last_count);
	else
		priv->counter += (NPCM_TIMER_INIT_VAL + 1 - priv->last_count) + val;
	priv->last_count = val;

	return priv->counter;
}

static int npcm_timer_probe(struct udevice *dev)
{
	struct timer_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	struct npcm_timer_priv *priv = dev_get_priv(dev);
	struct clk clk;
	int ret;

	priv->base = dev_read_addr_ptr(dev);
	if (!priv->base)
		return -ENOENT;
	priv->counter = 0;
	priv->last_count = 0;
	uc_priv->clock_rate = NPCM_TIMER_CLOCK_RATE;

	if (IS_ENABLED(CONFIG_ARCH_NPCM750)) {
		ret = clk_get_by_index(dev, 0, &clk);
		if (ret < 0)
			return ret;

		ret = clk_set_rate(&clk, NPCM_TIMER_INPUT_RATE);
		if (ret < 0)
			return ret;
	}

	/*
	 * Configure timer and start
	 * periodic mode
	 * input clock freq = 25Mhz
	 * prescale = 25
	 * clock rate = 25Mhz/25 = 1Mhz
	 */
	writel(0, priv->base + TCR0);
	writel(NPCM_TIMER_INIT_VAL, priv->base + TICR0);
	writel(TCR_EN | TCR_MODE_PERIODIC | TCR_PRESCALE_25,
	       priv->base + TCR0);

	return 0;
}

static const struct timer_ops npcm_timer_ops = {
	.get_count = npcm_timer_get_count,
};

static const struct udevice_id npcm_timer_ids[] = {
	{ .compatible = "nuvoton,npcm845-timer" },
	{ .compatible = "nuvoton,npcm750-timer" },
	{}
};

U_BOOT_DRIVER(npcm_timer) = {
	.name	= "npcm_timer",
	.id	= UCLASS_TIMER,
	.of_match = npcm_timer_ids,
	.priv_auto = sizeof(struct npcm_timer_priv),
	.probe = npcm_timer_probe,
	.ops	= &npcm_timer_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
