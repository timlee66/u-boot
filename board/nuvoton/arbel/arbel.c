// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gcr.h>
#include <asm/arch/clock.h>
#include <asm/mach-types.h>
#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <asm/gpio.h>
#include <asm/arch/espi.h>
#include <linux/delay.h>

DECLARE_GLOBAL_DATA_PTR;
/*
 * Routine: get_board_version_id
 * Description: Detect board version by reading  GPIO79 (VER_ID1), GPIO78 (VER_ID0).
 *		GPIO79 (VER_ID1), GPIO78 (VER_ID0): 1 1 => X00
 *		GPIO79 (VER_ID1), GPIO78 (VER_ID0): 1 0 => X01
 *		GPIO79 (VER_ID1), GPIO78 (VER_ID0): 0 1 => Reserved
 *		GPIO79 (VER_ID1), GPIO78 (VER_ID0): 0 0 => Reserved
 */
static int get_board_version_id(void)
{
	int pcb_version = 0;

	if (!gpio_request(PCB_VER_ID0, "rev0") &&
	    !gpio_request(PCB_VER_ID1, "rev1")) {
		gpio_direction_input(PCB_VER_ID0);
		gpio_direction_input(PCB_VER_ID1);

		pcb_version = gpio_get_value(PCB_VER_ID1) << 1 | gpio_get_value(PCB_VER_ID0);

		switch (pcb_version) {
		case 3:
			printf("NPCM845 EVB PCB version ID 0x%01x -> version X00\n",
			       pcb_version);
		break;
		case 2:
			printf("NPCM845 EVB PCB version ID 0x%01x -> version X01\n",
			       pcb_version);
		break;

		default:
			printf("NPCM845 EVB PCB version ID 0x%01x -> unknown version ID\n",
			       pcb_version);
		break;
		}
		gpio_free(PCB_VER_ID0);
		gpio_free(PCB_VER_ID1);
	} else {
		printf("Error: unable to acquire board version GPIOs\n");
	}

	return pcb_version;
}

static void espi_config(u8 mode, u8 max_freq, u32 ch_supp)
{
	u32 val;

	val = readl(NPCM_ESPI_BA + ESPICFG);
	val |= mode << ESPICFG_IOMODE_SHIFT;
	val |= max_freq << ESPICFG_MAXFREQ_SHIFT;
	val |= ((ch_supp & ESPICFG_CHNSUPP_MASK) << ESPICFG_CHNSUPP_SHFT);
	writel(val, NPCM_ESPI_BA + ESPICFG);
}

static void arbel_usb_init(void)
{
	struct clk_ctl *clkctl = (struct clk_ctl *)npcm_get_base_clk();
	struct npcm_gcr *gcr = (struct npcm_gcr *)npcm_get_base_gcr();

	/* Set usb host controller to reset */
	writel(readl(&clkctl->ipsrst2) | (1 << IPSRST2_USBHOST1), &clkctl->ipsrst2);
	writel(readl(&clkctl->ipsrst4) | (1 << IPSRST4_USBHOST2), &clkctl->ipsrst4);
	/* Set usb hub to reset */
	writel(readl(&clkctl->ipsrst3) | (1 << IPSRST3_USBHUB), &clkctl->ipsrst3);

	/* Set usb device controller to reset */
	writel(readl(&clkctl->ipsrst3) | (1 << IPSRST3_USBDEV9), &clkctl->ipsrst3);
	writel(readl(&clkctl->ipsrst3) | (1 << IPSRST3_USBDEV8), &clkctl->ipsrst3);
	writel(readl(&clkctl->ipsrst3) | (1 << IPSRST3_USBDEV7), &clkctl->ipsrst3);
	writel(readl(&clkctl->ipsrst1) | (1 << IPSRST1_USBDEV6), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) | (1 << IPSRST1_USBDEV5), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) | (1 << IPSRST1_USBDEV4), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) | (1 << IPSRST1_USBDEV3), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) | (1 << IPSRST1_USBDEV2), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) | (1 << IPSRST1_USBDEV1), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst3) | (1 << IPSRST3_USBDEV0), &clkctl->ipsrst3);

	/* Set usb phy1/2/3 to reset */
	writel(readl(&clkctl->ipsrst4) | (1 << IPSRST4_USBPHY3), &clkctl->ipsrst4);
	writel(readl(&clkctl->ipsrst3) | (1 << IPSRST3_USBPHY2), &clkctl->ipsrst3);
	writel(readl(&clkctl->ipsrst3) | (1 << IPSRST3_USBPHY1), &clkctl->ipsrst3);

	writel(readl(&gcr->usb1phyctl) & ~(1 << USB1PHYCTL_RS), &gcr->usb1phyctl);
	writel(readl(&gcr->usb2phyctl) & ~(1 << USB2PHYCTL_RS), &gcr->usb2phyctl);
	writel(readl(&gcr->usb3phyctl) & ~(1 << USB3PHYCTL_RS), &gcr->usb3phyctl);
	udelay(1000);

	/* Enable phy1/2/3 */
	writel(readl(&clkctl->ipsrst4) & ~(1 << IPSRST4_USBPHY3), &clkctl->ipsrst4);
	writel(readl(&clkctl->ipsrst3) & ~(1 << IPSRST3_USBPHY2), &clkctl->ipsrst3);
	writel(readl(&clkctl->ipsrst3) & ~(1 << IPSRST3_USBPHY1), &clkctl->ipsrst3);

	/* Wait for PHY clocks to stablize for 50us or more */
	udelay(1000);

	/* Set RS bit after PHY reset bit is released */
	writel(readl(&gcr->usb1phyctl) | (1 << USB1PHYCTL_RS), &gcr->usb1phyctl);
	writel(readl(&gcr->usb2phyctl) | (1 << USB2PHYCTL_RS), &gcr->usb2phyctl);
	writel(readl(&gcr->usb3phyctl) | (1 << USB3PHYCTL_RS), &gcr->usb3phyctl);

	/* Enable hub */
	writel(readl(&clkctl->ipsrst3) & ~(1 << IPSRST3_USBHUB), &clkctl->ipsrst3);
	/* Enable usb devices */
	writel(readl(&clkctl->ipsrst3) & ~((1 << IPSRST3_USBDEV9)), &clkctl->ipsrst3);
	writel(readl(&clkctl->ipsrst3) & ~((1 << IPSRST3_USBDEV8)), &clkctl->ipsrst3);
	writel(readl(&clkctl->ipsrst3) & ~((1 << IPSRST3_USBDEV7)), &clkctl->ipsrst3);
	writel(readl(&clkctl->ipsrst1) & ~((1 << IPSRST1_USBDEV6)), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) & ~((1 << IPSRST1_USBDEV5)), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) & ~((1 << IPSRST1_USBDEV4)), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) & ~((1 << IPSRST1_USBDEV3)), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) & ~((1 << IPSRST1_USBDEV2)), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) & ~((1 << IPSRST1_USBDEV1)), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst3) & ~((1 << IPSRST3_USBDEV0)), &clkctl->ipsrst3);
	/* Enable host 1/2 */
	writel(readl(&clkctl->ipsrst2) & ~(1 << IPSRST2_USBHOST1), &clkctl->ipsrst2);
	writel(readl(&clkctl->ipsrst4) & ~(1 << IPSRST4_USBHOST2), &clkctl->ipsrst4);
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

	/* Power voltage select setup */
	val = readl(&gcr->vsrcr);
	writel(val | BIT(30), &gcr->vsrcr);

	/* EVB X00 version - need to swap sgmii lane polarity HW issue */
	if (get_board_version_id() == 3) {
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
	arbel_usb_init();

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

int board_early_init_f(void)
{
	struct clk_ctl *clkctl = (struct clk_ctl *)(uintptr_t)npcm_get_base_clk();

	/* init uart clock */
	writel((readl(&clkctl->clkdiv1) & ~(0x1f << CLKDIV1_UARTDIV))
		| (19 << CLKDIV1_UARTDIV), &clkctl->clkdiv1);
	writel((readl(&clkctl->clksel) & ~(3 << CLKSEL_UARTCKSEL))
		| (CLKSEL_UARTCKSEL_PLL2 << CLKSEL_UARTCKSEL),
		&clkctl->clksel);

	return 0;
}
