/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#include <common.h>
#include <dm.h>
#include <sdhci.h>
#include <clk.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gcr.h>
#include <asm/arch/clock.h>

#if defined (CONFIG_ARCH_NPCM8XX)
#define NPCM_EMMC        0
#define NPCM_SD         -1    // SD card not existed in Arbel
#elif defined (CONFIG_TARGET_POLEG)
#define NPCM_SD          0
#define NPCM_EMMC        1
#endif

#ifdef CONFIG_DM_MMC
struct npcm_sdhci_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

DECLARE_GLOBAL_DATA_PTR;
#endif

static int npcm_sdhci_init(int index)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)npcm_get_base_gcr();
	struct clk_ctl *clkctl = (struct clk_ctl *)npcm_get_base_clk();

	if (index == NPCM_SD) {
#if defined (CONFIG_TARGET_POLEG)
		writel(readl(&gcr->mfsel3) | (1 << MFSEL3_SD1SEL), &gcr->mfsel3);
#endif
		writel(readl(&clkctl->ipsrst2) | (1 << IPSRST2_SDHC), &clkctl->ipsrst2);
		writel(readl(&clkctl->ipsrst2) & ~(1 << IPSRST2_SDHC), &clkctl->ipsrst2);
	}
	else if (index == NPCM_EMMC) {

#if defined (CONFIG_ARCH_NPCM8XX)
		writel(readl(&gcr->flockr2) & ~(1 << FLOCKR2_MMCRST), &gcr->flockr2);
		writel(readl(&gcr->flockr2) | (1 << FLOCKR2_MMCRST), &gcr->flockr2);
#ifdef _NPCM_EXT_SD_
		writel(readl((volatile uint32_t *)(0xf0800efc)) | (1 << 6), (volatile uint32_t *)(0xf0800efc));
		printf("BIT6=1 SD  SCRCHPAD63=0x%x  \n", *(volatile uint32_t *)(0xf0800efc));
#else
		writel(readl((volatile uint32_t *)(0xf0800efc)) & ~(1 << 6), (volatile uint32_t *)(0xf0800efc));
		printf("BIT6=0 eMMC  SCRCHPAD63=0x%x  \n", *(volatile uint32_t *)(0xf0800efc));
#endif
#endif

#if defined (CONFIG_ARCH_NPCM8XX)
#ifdef _NPCM_EXT_SD_
		writel((readl(&gcr->mfsel3) & ~(1 << MFSEL3_MMCCDSEL) ) | (1 << MFSEL3_MMCSEL), &gcr->mfsel3);
#else
		writel((readl(&gcr->mfsel3) & ~(1 << MFSEL3_MMCCDSEL) ) | (1 << MFSEL3_MMCSEL) | (1 << MFSEL3_MMC8SEL), &gcr->mfsel3);
#endif
#elif defined (CONFIG_TARGET_POLEG)
		writel((readl(&gcr->mfsel3) & ~(1 << MFSEL3_MMCCDSEL) ) | (1 << MFSEL3_MMCSEL) | (1 << MFSEL3_MMC8SEL), &gcr->mfsel3);
		writel((readl(&gcr->mfsel4) | (1 << MFSEL4_MMCRSEL)), &gcr->mfsel4);
#endif
		writel(readl(&clkctl->ipsrst2) | (1 << IPSRST2_MMC), &clkctl->ipsrst2);
		writel(readl(&clkctl->ipsrst2) & ~(1 << IPSRST2_MMC), &clkctl->ipsrst2);
	}
	return 0;
}

#ifdef CONFIG_DM_MMC
static int npcm_sdhci_probe(struct udevice *dev)
{
	struct npcm_sdhci_plat *plat = dev_get_plat(dev);
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	int ret;

	ret = npcm_sdhci_init(host->index);

	if (ret)
		return ret;

	host->quirks = SDHCI_QUIRK_BROKEN_R1B | SDHCI_QUIRK_WAIT_SEND_CMD |
	               SDHCI_QUIRK_32BIT_DMA_ADDR | SDHCI_QUIRK_USE_WIDE8;

	host->voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;

#if defined (CONFIG_TARGET_POLEG)
	if (host->index == NPCM_SD)
#endif
	{
#if defined (CONFIG_TARGET_POLEG) || defined (_NPCM_EXT_SD_)
		unsigned int status;

		sdhci_writeb(host, SDHCI_CTRL_CD_TEST_INS | SDHCI_CTRL_CD_TEST,
			SDHCI_HOST_CONTROL);

		status = sdhci_readl(host, SDHCI_PRESENT_STATE);
		while ((!(status & SDHCI_CARD_PRESENT)) ||
		    (!(status & SDHCI_CARD_STATE_STABLE)) ||
		    (!(status & SDHCI_CARD_DETECT_PIN_LEVEL)))
			status = sdhci_readl(host, SDHCI_PRESENT_STATE);
#endif
	}

	host->version = sdhci_readw(host, SDHCI_HOST_VERSION);
	if (host->bus_width == 4)
		host->host_caps |= MMC_MODE_4BIT;

	if (host->bus_width == 8)
		host->host_caps |= MMC_MODE_8BIT;

	host->mmc = &plat->mmc;
	host->mmc->priv = host;
	host->mmc->dev = dev;
	upriv->mmc = host->mmc;

	ret = sdhci_setup_cfg(&plat->cfg, host, host->clock, 400000);
	if (ret)
		return ret;

	host->clock = 0;
	return sdhci_probe(dev);
}

static int npcm_ofdata_to_platdata(struct udevice *dev)
{
	struct sdhci_host *host = dev_get_priv(dev);

	host->name = strdup(dev->name);
	host->ioaddr = (void *)dev_read_addr(dev);
#ifdef _NPCM_EXT_SD_
	host->bus_width = 4;
#else
	host->bus_width = fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev),
			"bus-width", 4);
#endif

	host->index = fdtdec_get_uint(gd->fdt_blob, dev_of_offset(dev), "index", 0);
	host->clock = fdtdec_get_uint(gd->fdt_blob, dev_of_offset(dev),
			"clock-frequency", 400000);

	if (host->ioaddr == (void *)FDT_ADDR_T_NONE)
		return -EINVAL;

	return 0;
}

static int npcm_sdhci_bind(struct udevice *dev)
{
	struct npcm_sdhci_plat *plat = dev_get_plat(dev);
	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static int npcm_sdhci_remove(struct udevice *dev)
{
	return 0;
}

static const struct udevice_id npcm_mmc_ids[] = {
	{ .compatible = "nuvoton,npcm845-sdhci-SD" },
	{ .compatible = "nuvoton,npcm845-sdhci-eMMC"},
	{ }
};

U_BOOT_DRIVER(npcm_sdc_drv) = {
	.name           = "npcm_sdhci",
	.id             = UCLASS_MMC,
	.of_match       = npcm_mmc_ids,
	.of_to_plat     = npcm_ofdata_to_platdata,
	.ops            = &sdhci_ops,
	.bind           = npcm_sdhci_bind,
	.probe          = npcm_sdhci_probe,
	.remove         = npcm_sdhci_remove,
	.priv_auto      = sizeof(struct sdhci_host),
	.plat_auto      = sizeof(struct npcm_sdhci_plat),
};
#endif
