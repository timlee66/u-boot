// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#include <common.h>
#include <dm.h>
#include <env.h>
#include <fdtdec.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/espi.h>
#include <asm/arch/gcr.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <linux/delay.h>

DECLARE_GLOBAL_DATA_PTR;

static void espi_config(u8 mode, u8 max_freq, u32 ch_supp)
{
	u32 val;

	val = readl(NPCM_ESPI_BA + ESPICFG);
	val |= mode << ESPICFG_IOMODE_SHIFT;
	val |= max_freq << ESPICFG_MAXFREQ_SHIFT;
	val |= ((ch_supp & ESPICFG_CHNSUPP_MASK) << ESPICFG_CHNSUPP_SHFT);
	writel(val, NPCM_ESPI_BA + ESPICFG);
}

static void arbel_sysintf_init(void)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)(uintptr_t)npcm_get_base_gcr();
	u32 espi_ch_supp;

	espi_ch_supp = fdtdec_get_config_int(gd->fdt_blob, "espi-channel-support", 0);

	if (espi_ch_supp > 0) {
		u32 hindp = 0x00011110 | espi_ch_supp;
		/* Set ESPI_SEL bit in MFSEL4 register */
		writel((readl(&gcr->mfsel4) | (1 << MFSEL4_ESPISEL)), &gcr->mfsel4);

		/*
		 * In eSPI HOST INDEPENDENCE register, set bits
		 * AUTO_SBLD, AUTO_FCARDY, AUTO_OOBCRDY,
		 * AUTO_VWCRDY, AUTO_PCRDY, AUTO_HS1, AUTO_HS2, AUTO_HS3.
		 */
		writel(hindp, NPCM_ESPI_BA + ESPIHINDP);

		/*
		 * In eSPI ESPICFG register set ESPICFG.MAXREQ to 33 MHz and ESPICFG. IOMODE
		 * to Quad.
		 */
		espi_config(ESPI_IO_MODE_SINGLE_DUAL_QUAD, ESPI_MAX_33_MHZ, espi_ch_supp);
	} else {
		/* set LPCSEL bit in MFSEL1 register */
		writel((readl(&gcr->mfsel1) | (1 << MFSEL1_LPCSEL)), &gcr->mfsel1);
	}
}

static void arbel_eth_init(void)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)(uintptr_t)npcm_get_base_gcr();
	u32 val;
	char *evb_ver;

	/* Power voltage select setup */
	val = readl(&gcr->vsrcr);
	writel(val | BIT(30), &gcr->vsrcr);

	/* EVB X00 version - need to swap sgmii lane polarity HW issue */
	evb_ver = env_get("evb_version");
	if (evb_ver && !strcmp(evb_ver, "X00")) {
		/* Get access to 0x3F... (VR_MII_MMD_DIG_CTRL1) */
		writew(0x1F80, 0xF07801FE);
		/* Swap lane polarity on EVB only */
		writew(readw(0xf07801c2) | BIT(0), 0xf07801c2);
		/* Set SGMII MDC/MDIO pins to output slew-rate high */
		writel(readl(0xf001305) | 0x3000, 0xf001305c);
		printf("EVB-X00 SGMII Work-Around\n");
	}
}

int board_init(void)
{
	arbel_sysintf_init();
	arbel_eth_init();

	gd->bd->bi_arch_number = CONFIG_MACH_TYPE;
	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100UL);

	return 0;
}

int dram_init(void)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)npcm_get_base_gcr();

	/*
	 * get dram active size value from bootblock.
	 * Value sent using scrpad_02 register.
	 * feature available in bootblock 0.0.6 and above.
	 */
	gd->ram_size = readl(&gcr->scrpad_b);

	return 0;
}
