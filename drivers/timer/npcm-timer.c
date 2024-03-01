// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022 Nuvoton Technology Corp.
 */

#include <common.h>
#include <dm.h>
#include <timer.h>
#include <asm/io.h>

#define NPCM_TIMER_CLOCK_RATE	25000000UL	/* 25MHz */

/* Register offsets */
#define SECCNT	0x0	/* Seconds Counter Register */
#define CNTR25M	0x4	/* Counter 25M Register */

struct npcm_timer_priv {
	void __iomem *base;
};

static u64 npcm_timer_get_count(struct udevice *dev)
{
	struct npcm_timer_priv *priv = dev_get_priv(dev);
	u64 counter;

	counter = readl(priv->base + SECCNT) * NPCM_TIMER_CLOCK_RATE;
	counter += readl(priv->base + CNTR25M);

	return counter;
}

static int npcm_timer_probe(struct udevice *dev)
{
	struct timer_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	struct npcm_timer_priv *priv = dev_get_priv(dev);

	priv->base = dev_read_addr_ptr(dev);
	if (!priv->base)
		return -EINVAL;
	uc_priv->clock_rate = NPCM_TIMER_CLOCK_RATE;

	return 0;
}

static const struct timer_ops npcm_timer_ops = {
	.get_count = npcm_timer_get_count,
};

static const struct udevice_id npcm_timer_ids[] = {
	{ .compatible = "nuvoton,npcm845-timer"},
	{ .compatible = "nuvoton,npcm750-timer"},
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
