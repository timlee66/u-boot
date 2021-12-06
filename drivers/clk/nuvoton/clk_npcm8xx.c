// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology.
 */

#include <common.h>
#include <asm/types.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/io.h>
#include <dm.h>
#include <clk-uclass.h>
#include <linux/delay.h>
#include <dt-bindings/clock/npcm845-clock.h>

struct npcm_clk_priv {
	struct clk_ctl *regs;
};

enum pll_clks {
	PLL_0,
	PLL_1,
	PLL_2,
	PLL_CLKREF,
};

static u32 clk_get_pll_freq(struct clk_ctl *pll_clk, enum pll_clks pll)
{
	u32 pllval;
	u32 fin = EXT_CLOCK_FREQUENCY_KHZ; /* 25KHz */
	u32 fout, nr, nf, no;

	switch (pll) {
	case PLL_0:
		pllval = readl(&pll_clk->pllcon0);
		break;
	case PLL_1:
		pllval = readl(&pll_clk->pllcon1);
		break;
	case PLL_2:
		pllval = readl(&pll_clk->pllcon2);
		break;
	case PLL_CLKREF:
	default:
		return 0;
	}

	/* PLL Input Clock Divider */
	nr = (pllval >> PLLCON_INDV) & 0x1f;
	/* PLL VCO Output Clock Feedback Divider */
	nf = (pllval >> PLLCON_FBDV) & 0xfff;
	/* PLL Output Clock Divider 1 */
	no = ((pllval >> PLLCON_OTDV1) & 0x7) *
		((pllval >> PLLCON_OTDV2) & 0x7);

	fout = ((10 * fin * nf) / (no * nr));

	return fout * 100;
}

static u32 npcm_mmc_set_rate(struct clk_ctl *mmc_clk, ulong rate)
{
	u32 pll0_freq, divider, sdhci_clk;

	/* To acquire PLL0 frequency. */
	pll0_freq = clk_get_pll_freq(mmc_clk, PLL_0);

	/* Calculate rounded up divider to produce closest to
	 * target output clock
	 */
	divider = (pll0_freq % rate == 0) ? (pll0_freq / rate) :
						(pll0_freq / rate) + 1;

	writel((readl(&mmc_clk->clkdiv1) & ~(0x1f << CLKDIV1_MMCCKDIV))
	       | (divider - 1) << CLKDIV1_MMCCKDIV, &mmc_clk->clkdiv1);

	/* Wait to the divider to stabilize */
	udelay(100);

	/* Select PLL0 as source */
	writel((readl(&mmc_clk->clksel) & ~(0x3 << CLKSEL_SDCKSEL))
		| (CLKSEL_SDCKSEL_PLL0 << CLKSEL_SDCKSEL),
		&mmc_clk->clksel);

	sdhci_clk = pll0_freq / divider;

	return sdhci_clk;
}

static ulong npcm_get_cpu_freq(struct clk_ctl *cpu_clk)
{
	ulong fout = 0;
	u32 clksel = readl(&cpu_clk->clksel) & (0x3 << CLKSEL_CPUCKSEL);

	if (clksel == CLKSEL_CPUCKSEL_PLL0)
		fout = (ulong)clk_get_pll_freq(cpu_clk, PLL_0);
	else if (clksel == CLKSEL_CPUCKSEL_PLL1)
		fout = (ulong)clk_get_pll_freq(cpu_clk, PLL_1);
	else if (clksel == CLKSEL_CPUCKSEL_CLKREF)
		fout = EXT_CLOCK_FREQUENCY_MHZ; /* FOUT is specified in MHz */
	else
		fout = EXT_CLOCK_FREQUENCY_MHZ; /* FOUT is specified in MHz */

	return fout;
}

static u32 npcm_get_apb_divisor(struct clk_ctl *regs, u32 apb)
{
	u32 apb_divisor = 2;

	/* AHBn divider */
	apb_divisor = apb_divisor * (1 << ((readl(&regs->clkdiv1)
						>> CLKDIV1_CLK4DIV) & 0x3));

	switch (apb) {
	case APB2: /* APB divisor */
		apb_divisor = apb_divisor *
				(1 << ((readl(&regs->clkdiv2)
					>> CLKDIV2_APB2CKDIV) & 0x3));
		break;
	case APB5: /* APB divisor */
		apb_divisor = apb_divisor *
				(1 << ((readl(&regs->clkdiv2)
					>> CLKDIV2_APB5CKDIV) & 0x3));
		break;
	default:
		apb_divisor = 0xFFFFFFFF;
		break;
	}

	return apb_divisor;
}

static ulong npcm_clk_get_rate(struct clk *clk)
{
	struct npcm_clk_priv *priv = dev_get_priv(clk->dev);

	switch (clk->id) {
	case CLK_APB2:
		return npcm_get_cpu_freq(priv->regs) /
			npcm_get_apb_divisor(priv->regs, APB2);
	case CLK_APB5:
		return npcm_get_cpu_freq(priv->regs) /
			npcm_get_apb_divisor(priv->regs, APB5);
	}

	return 0;
}

static ulong npcm_clk_set_rate(struct clk *clk, ulong rate)
{
	struct npcm_clk_priv *priv = dev_get_priv(clk->dev);

	switch (clk->id) {
	case CLK_EMMC:
		return npcm_mmc_set_rate(priv->regs, rate);

	default:
		return -ENOENT;
	}

	return 0;
}

static int npcm_clk_probe(struct udevice *dev)
{
	struct npcm_clk_priv *priv = dev_get_priv(dev);
	void *base;

	base = dev_read_addr_ptr(dev);
	if (!base)
		return -ENOENT;

	priv->regs = (struct clk_ctl *)base;

	return 0;
}

static struct clk_ops npcm_clk_ops = {
	.get_rate = npcm_clk_get_rate,
	.set_rate = npcm_clk_set_rate,
};

static const struct udevice_id npcm_clk_ids[] = {
	{ .compatible = "nuvoton,npcm845-clock" },
	{ }
};

U_BOOT_DRIVER(clk_npcm) = {
	.name           = "clk_npcm",
	.id             = UCLASS_CLK,
	.of_match       = npcm_clk_ids,
	.ops            = &npcm_clk_ops,
	.priv_auto	= sizeof(struct npcm_clk_priv),
	.probe          = npcm_clk_probe,
};
