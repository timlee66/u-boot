/*
 * NUVOTON Poleg RNG driver
 *
 * Copyright (C) 2019, NUVOTON, Incorporated
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <uboot_aes.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/poleg_rng.h>

struct npcm750_rng_priv {
	struct poleg_rng_regs* regs;
};

static struct npcm750_rng_priv *rng_priv;

void npcm750_rng_init(void)
{
	struct poleg_rng_regs *regs = rng_priv->regs;
	int init;

	/* check if rng enabled */
	init = readb(&regs->rngcs);
	if ((init & RNGCS_RNGE) == 0) {
		/* init rng */
		writeb(RNGCS_CLKP(RNG_CLKP_20_25_MHz) | RNGCS_RNGE, &(regs->rngcs));
		writeb(RNGMODE_M1ROSEL_VAL, &regs->rngmode);
	}
}

void npcm750_rng_disable(void)
{
	struct poleg_rng_regs *regs = rng_priv->regs;

	/* disable rng */
	writeb(0, &regs->rngcs);
	writeb(0, &regs->rngmode);
}

void srand(unsigned int seed)
{
	/* no need to seed for now */
	return;
}

unsigned int rand_r(unsigned int *seedp)
{
	struct poleg_rng_regs *regs = rng_priv->regs;
	int  i;
	unsigned int ret_val = 0;

	npcm750_rng_init();

	/* Wait for RNG done (4 bytes) */
	for (i = 0; i < 4 ;i++) {
		 /* wait until DVALID is set */
		while ((readb(&regs->rngcs) & RNGCS_DVALID) == 0);
		ret_val |= (((unsigned int)readb(&regs->rngd) & 0x000000FF) << (i * 8));
	}

	return ret_val;
}

unsigned int rand(void)
{
	return rand_r(NULL);
}

static int npcm750_rng_bind(struct udevice *dev)
{
	rng_priv = calloc(1, sizeof(struct npcm750_rng_priv));
	if (!rng_priv)
		return -ENOMEM;

	rng_priv->regs = dev_remap_addr_index(dev, 0);
	if (!rng_priv->regs) {
		printf("Cannot find rng reg address, binding failed\n");
		return -EINVAL;
	}

	printk(KERN_INFO "RNG: NPCM750 RNG module bind OK\n");

	return 0;
}

static const struct udevice_id npcm750_rng_ids[] = {
	{ .compatible = "nuvoton,npcm750-rng" },
	{ }
};

U_BOOT_DRIVER(npcm750_rng) = {
	.name = "npcm750_rng",
	.id = UCLASS_MISC,
	.of_match = npcm750_rng_ids,
	.priv_auto_alloc_size = sizeof(struct npcm750_rng_priv),
	.bind = npcm750_rng_bind,
};
