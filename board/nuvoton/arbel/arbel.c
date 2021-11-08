/*
 * Copyright (c) 2016-2021 Nuvoton Technology Corp.
 *
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gcr.h>
#include <asm/mach-types.h>
#include <asm/arch/clock.h>
#include <asm/arch/otp.h>
#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <clk.h>
#include <fuse.h>
#include <spi_flash.h>
#include <spi.h>
#include <asm/gpio.h>
#include <asm/arch/espi.h>
#include <linux/delay.h>

DECLARE_GLOBAL_DATA_PTR;

extern void sdelay(unsigned long loops);


#define SR_MII_CTRL_ANEN_BIT12  12
#define SR_MII_CTRL_SWR_BIT15   15
#define VR_MII_MMD_DIG_CTRL1_R2TLBE_BIT14 14
#define VR_MII_MDD_DIG_CTRL2_RX_POL_INV_0_BIT0 0

/*
 * Routine: get_board_version_id
 * Description: Detect board version by reading  GPIO79 (VER_ID1), GPIO78 (VER_ID0).
 *		GPIO79 (VER_ID1), GPIO78 (VER_ID0): 1 1 => X00
 *		GPIO79 (VER_ID1), GPIO78 (VER_ID0): 1 0 => X01
 *		GPIO79 (VER_ID1), GPIO78 (VER_ID0): 0 1 => Reserved
 *		GPIO79 (VER_ID1), GPIO78 (VER_ID0): 0 0 => Reserved
 */
int get_board_version_id(void)
{
	static int pcb_version = -1;

	if (pcb_version == -1)
	{
		if (!gpio_request(PCB_VER_ID0, "rev0") &&
		    !gpio_request(PCB_VER_ID1, "rev1"))
	    {
			gpio_direction_input(PCB_VER_ID0);
			gpio_direction_input(PCB_VER_ID1);

			pcb_version = gpio_get_value(PCB_VER_ID1) << 1 | gpio_get_value(PCB_VER_ID0);

			switch(pcb_version)
			{
				case 3:
					printf("NPCM845 EVB PCB version ID 0x%01x -> version X00 \n", pcb_version);
				break;
				case 2:
					printf("NPCM845 EVB PCB version ID 0x%01x -> version X01 \n", pcb_version);
				break;

				default:
					printf("NPCM845 EVB PCB version ID 0x%01x -> unknown version ID \n", pcb_version);
				break;
			}
			gpio_free(PCB_VER_ID0);
			gpio_free(PCB_VER_ID1);
		} else {
			printf("Error: unable to acquire board version GPIOs\n");
		}
	}
	return pcb_version;
}

void ESPI_ConfigAutoHandshake (u32 AutoHsCfg)
{
	writel( AutoHsCfg,
	        NPCM_ESPI_BA + ESPIHINDP);
}

void ESPI_Config (
        ESPI_IO_MODE  ioMode,
        ESPI_MAX_FREQ maxFreq,
        u32           ch_supp
)
{
	u32 var = readl(NPCM_ESPI_BA + ESPICFG);
	var |= ioMode << ESPICFG_IOMODE;
	var |= maxFreq << ESPICFG_MAXFREQ;
	var |= ((ch_supp & ESPICFG_CHNSUPP_MASK) << ESPICFG_CHNSUPP_SHFT);
	writel( var,
	        NPCM_ESPI_BA + ESPICFG);
}

int board_init(void)
{
	u32 espi_ch_supp;

#ifdef CONFIG_ETH_DESIGNWARE
	unsigned int start;

	struct clk_ctl *clkctl = (struct clk_ctl *)(uintptr_t)npcm_get_base_clk();
	struct npcm_gcr *gcr = (struct npcm_gcr *)(uintptr_t)npcm_get_base_gcr();

    /* Power voltage select setup  TBD  move to dts */
	writel( 0x40004800, &gcr->vsrcr);

    /* Clock setups */
	writel((readl(&clkctl->clkdiv2) & ~(0x1f << CLKDIV2_CLKOUTDIV)) | (31 << CLKDIV2_CLKOUTDIV), &clkctl->clkdiv2);
	sdelay(420000UL); 	/* udelay(200) */
	writel((readl(&clkctl->clksel) & ~(0x7 << CLKSEL_CLKOUTSEL)) | (CLKSEL_CLKOUTSEL_PLL0 << CLKSEL_CLKOUTSEL), &clkctl->clksel);  // Select PLL0 for CLKOUTSEL

	writel(readl(&gcr->flockr2) | (0x1 << FLOCKR2_G35DA2P), &gcr->flockr2);


//#define GMAC1_SGMII_PCS_LB


	/* Enable SGMII/RGMII for GMAC1/2 module */
	writel((readl(&gcr->mfsel4) | (1 << MFSEL4_SG1MSEL)), &gcr->mfsel4);     // GMAC1 - MDIO SGMII

#ifdef GMAC2_RGMII
	writel((readl(&gcr->mfsel4) | (1 << MFSEL4_RG2SEL)), &gcr->mfsel4);      // GMAC2 - RGMII
	writel((readl(&gcr->mfsel4) | (1 << MFSEL4_RG2MSEL)), &gcr->mfsel4);     // GMAC2 - MDIO

#else  /* GMAC2 RMII3 */
	writel((readl(&gcr->mfsel4) & ~(1 << MFSEL4_RG2SEL)), &gcr->mfsel4);     // Switch GMAC2 to RMII3 Mode
	writel((readl(&gcr->mfsel3) & ~(1 << MFSEL3_DDRDVOSEL)), &gcr->mfsel3);  // Switch GMAC2 to RMII3 Mode
	writel((readl(&gcr->mfsel4) | (1 << MFSEL4_RG2MSEL)), &gcr->mfsel4);     // GMAC2 - MDIO
	writel((readl(&gcr->mfsel5) | (1 << MFSEL5_RMII3SEL)), &gcr->mfsel5);    // RMII3 select
	writel((readl(&gcr->intcr4) | (1 << INTCR4_R3EN)), &gcr->intcr4);        // RMII3 Set INTCR4_R3EN Enable
	writel((readl(&gcr->mfsel5) | (1 << MFSEL5_R3OENSEL)), &gcr->mfsel5);    // RMII3 Set MFSEL5_R3OENSEL Output-Enable
#endif /* #endif GMAC2_RGMII */

	writel((readl(&gcr->mfsel3) | (1 << MFSEL3_RMII1SEL)), &gcr->mfsel3);    // GMAC3 - RMII1 Select
	writel((readl(&gcr->intcr4) | (1 << INTCR4_R1EN)), &gcr->intcr4);        // GMAC3 - RMII1 Set INTCR4_R1EN
	writel((readl(&gcr->mfsel5) | (1 << MFSEL5_R1OENSEL)), &gcr->mfsel5);    // GMAC3 - RMII1 Set MFSEL5_R1OENSEL
	writel((readl(&gcr->mfsel1) & ~(1 << MFSEL1_R1ERRSEL)), &gcr->mfsel1);   // GMAC3 - RMII1 Clear MFSEL1_R1ERRSEL

	writel((readl(&gcr->mfsel1) | (1 << MFSEL1_RMII2SEL)), &gcr->mfsel1);    // GMAC4 - RMII2 Select
	writel((readl(&gcr->intcr4) | (1 << INTCR4_R2EN)), &gcr->intcr4);        // GMAC4 - RMII2 Set INTCR4_R2EN
	writel((readl(&gcr->mfsel5) | (1 << MFSEL5_R2OENSEL)), &gcr->mfsel5);    // GMAC4 - RMII2 Set MFSEL5_R2OENSEL
	writel((readl(&gcr->mfsel1) & ~(1 << MFSEL1_R2ERRSEL)), &gcr->mfsel1);   // GMAC4 - RMII2 Clear MFSEL1_R2ERRSEL

	/* IP Software Reset for GMAC1/2 module */
	writel(readl(&clkctl->ipsrst2) | (1 << IPSRST2_GMAC1), &clkctl->ipsrst2);
	writel(readl(&clkctl->ipsrst2) & ~(1 << IPSRST2_GMAC1), &clkctl->ipsrst2);
	writel(readl(&clkctl->ipsrst2) | (1 << IPSRST2_GMAC2), &clkctl->ipsrst2);
	writel(readl(&clkctl->ipsrst2) & ~(1 << IPSRST2_GMAC2), &clkctl->ipsrst2);

	/* IP Software Reset for GMAC3/4 module */
	writel(readl(&clkctl->ipsrst1) | (1 << IPSRST1_GMAC3), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) & ~(1 << IPSRST1_GMAC3), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) | (1 << IPSRST1_GMAC4), &clkctl->ipsrst1);
	writel(readl(&clkctl->ipsrst1) & ~(1 << IPSRST1_GMAC4), &clkctl->ipsrst1);


        /* SGMII PHY reset */
	writew(0x1F00, 0xF07801FE);           /* Get access to 0x3E... (SR_MII_CTRL) */
	writew(readw(0xF0780000) | (1 << SR_MII_CTRL_SWR_BIT15), 0xF0780000);
	start = get_timer(0);

	printf("SGMII PCS PHY reset wait \n");
	while (readw(0xF0780000) & (1 << SR_MII_CTRL_SWR_BIT15))
	{
		if (get_timer(start) >= 3*CONFIG_SYS_HZ)
		{
			printf("SGMII PHY reset timeout\n");
			return -ETIMEDOUT;
		}

		mdelay(1);
	};

	/* Clear SGMII PHY default auto neg. */
	writew(readw(0xF0780000) & ~(1 << SR_MII_CTRL_ANEN_BIT12), 0xF0780000);
	printf("SGMII PCS PHY reset done and clear Auto Negotiation \n");

#ifdef CONFIG_TARGET_ARBEL_EVB
    if (get_board_version_id() == 3 )  /* EVB X00 version - need to swap sgmii lane polarity HW issue */
	{
		writew(0x1F80, 0xF07801FE);                           /* Get access to 0x3F... (VR_MII_MMD_DIG_CTRL1) */
		writew(readw(0xf07801c2) | (1 << VR_MII_MDD_DIG_CTRL2_RX_POL_INV_0_BIT0), 0xf07801c2);                      /* Swap lane polarity on EVB only */
		writel(readl((volatile uint32_t *)(0xf001305c)) | 0x3000, (volatile uint32_t *)(0xf001305c));           	/* Set SGMII MDC/MDIO pins to output slew-rate high */
		printf("EVB-X00 SGMII Work-Around: RX Polarity Invert Lane-0 and MDC/MDIO pins output slew-rate high\n");
	}

	/* Set reg SMC_CTL bit HOSTWAIT Write 1 to Clear */
	writeb(readb((volatile uint8_t *)(0xC0001001)) | 0x80, (volatile uint8_t *)(0xC0001001));
#endif

#ifdef GMAC1_SGMII_PCS_LB
	writew(0x1F80, 0xF07801FE);           /* Get access to 0x3F... (VR_MII_MMD_DIG_CTRL1) */
	writew(readw(0xF0780000) | (1 << VR_MII_MMD_DIG_CTRL1_R2TLBE_BIT14), 0xF0780000);
#endif

	gd->bd->bi_arch_number = CONFIG_MACH_TYPE;
	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100UL);
#endif

	espi_ch_supp = fdtdec_get_config_int(gd->fdt_blob, "espi-channel-support", 0);

	if (espi_ch_supp > 0) {
	  u32 hindp = 0x00011110 | espi_ch_supp;
	  // Set ESPI_SEL bit in MFSEL4 register.
	  writel((readl(&gcr->mfsel4) | (1 << MFSEL4_ESPISEL)), &gcr->mfsel4);

	  // In eSPI HOST INDEPENDENCE register, set bits
	  // AUTO_SBLD, AUTO_FCARDY, AUTO_OOBCRDY,
	  // AUTO_VWCRDY, AUTO_PCRDY, AUTO_HS1, AUTO_HS2, AUTO_HS3.
	  ESPI_ConfigAutoHandshake(hindp);

	  // In eSPI ESPICFG register set ESPICFG.MAXREQ to 33 MHz and ESPICFG. IOMODE
	  // to Quad.
	  ESPI_Config(ESPI_IO_MODE_SINGLE_DUAL_QUAD, ESPI_MAX_33_MHz, espi_ch_supp);
	}
	else {
	  // set LPCSEL bit in MFSEL1 register.
	  writel((readl(&gcr->mfsel1) | (1 << MFSEL1_LPCSEL)), &gcr->mfsel1);
	}

	return 0;
}

int dram_init(void)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)(uintptr_t)npcm_get_base_gcr();

	// get dram active size value from bootblock. Value sent using scrpad_02 register.
	// feature available in bootblock 0.0.6 and above.
	gd->ram_size = readl(&gcr->scrpad_b);

	return 0;
}

#ifdef CONFIG_BOARD_EARLY_INIT_F
int board_early_init_f(void)
{
	return 0;
}
#endif


int board_eth_init(struct bd_info *bis)
{
	return 0;
}


#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	const char *board_info;

	board_info = fdt_getprop(gd->fdt_blob, 0, "model", NULL);
	printf("Board: %s\n", board_info ? board_info : "unknown");
#ifdef CONFIG_BOARD_TYPES
	board_info = get_board_type();
	if (board_info)
		printf("Type:  %s\n", board_info);
#endif
	return 0;
}
#endif

#ifdef CONFIG_LAST_STAGE_INIT
#ifdef SECURE_BOOT
static bool is_security_enabled(void)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)(uintptr_t)npcm_get_base_gcr();

	if ((readl(&gcr->pwron) & (1 << PWRON_SECEN))) {
		printf("Security is enabled\n");
		return true;
	} else {
		printf("Security is NOT enabled\n");
		return false;
	}
}

static int check_nist_version(void)
{
#if (CONFIG_NIST_VERSION_ADDR != 0)
	volatile u32 uboot_ver = *(u32*)(uintptr_t)(UBOOT_RAM_IMAGE_ADDR + HEADER_VERSION_OFFSET);
	volatile u32 nist_ver = *(u32*)(uintptr_t)(CONFIG_NIST_VERSION_ADDR);

	if (uboot_ver != nist_ver)
		reset_cpu(0);
#endif

	return 0;
}

static int secure_boot_configuration(void)
{
#if defined(CONFIG_SPI_FLASH) && defined(SPI_FLASH_BASE_ADDR)

	const u8 tag[SA_TAG_FLASH_IMAGE_SIZE] = SA_TAG_FLASH_IMAGE_VAL;
	struct spi_flash *flash;
	struct udevice *udev;
	u32 addr, addr_align;
	int rc , i, offset;
	u8 *buf = NULL;

	rc = spi_flash_probe_bus_cs(CONFIG_SF_DEFAULT_BUS, CONFIG_SF_DEFAULT_CS,
			CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE, &udev);
	if (rc)
		return rc;

	flash = dev_get_uclass_priv(udev);
	if (!flash)
		return -1;

	// fuse images should be a part of the flash image, right after the uboot
	addr = POLEG_UBOOT_END;

	// if found, program the image to the fuse arrays, set the secure boot
	// bit and erase the image from the flash
	if (((u32*)(uintptr_t)(addr + SA_TAG_FLASH_IMAGE_OFFSET))[0] == ((u32*)tag)[0] &&
		((u32*)(uintptr_t)(addr + SA_TAG_FLASH_IMAGE_OFFSET))[1] == ((u32*)tag)[1]) {

		u8 fuse_arrays[2 * NPCM_OTP_ARR_BYTE_SIZE];
		u32 fustrap_orig;

		printf("%s(): fuse array image was found on flash in address 0x%x\n", __func__, addr);

		memcpy(fuse_arrays, (u8*)(uintptr_t)addr, sizeof(fuse_arrays));

		fustrap_orig = *(u32*)(fuse_arrays + SA_FUSE_FLASH_IMAGE_OFFSET);

		//TODO: Here, randomize 4 AES keys + generate their nibble parity + embed to image

		printf("%s(): program fuse key array from address 0x%x\n", __func__, addr + SA_KEYS_FLASH_IMAGE_OFFSET);

		rc = fuse_prog_image(NPCM_KEY_SA, (u32)(uintptr_t)(fuse_arrays + SA_KEYS_FLASH_IMAGE_OFFSET));
		if (rc != 0)
			return rc;

		// clear oSecBoot, will be programmed only after everything is
		// programmed successfuly
		fustrap_orig = *(u32*)(uintptr_t)(addr + SA_FUSE_FLASH_IMAGE_OFFSET);
		*(u32*)(fuse_arrays + SA_FUSE_FLASH_IMAGE_OFFSET) &= ~FUSTRAP_O_SECBOOT;

		printf("%s(): program fuse strap array from address 0x%x\n", __func__, addr + SA_FUSE_FLASH_IMAGE_OFFSET);

		rc = fuse_prog_image(NPCM_FUSE_SA, (u32)(uintptr_t)(fuse_arrays + SA_FUSE_FLASH_IMAGE_OFFSET));
		if (rc != 0)
			return rc;

		// erase the whole sector
		addr_align = addr & ~(u32)(uintptr_t)(flash->erase_size -1);

		offset = addr - addr_align;
		printf("%s(): erase the sector of addr 0x%x\n", __func__, addr_align);

		addr_align -= SPI_FLASH_BASE_ADDR;

		buf = (u8 *)(uintptr_t)malloc(flash->erase_size);
		if (buf) {
			spi_flash_read(flash, addr_align, flash->erase_size, buf);
		} else {
			printf("%s(): failed to alloc buffer, skip otp program\n", __func__);
			return -1;
		}

		for (i = 0 ; i < (SA_TAG_FLASH_IMAGE_OFFSET + SA_TAG_FLASH_IMAGE_SIZE) ; i++)
			buf[offset + i] = 0xff;

		rc = spi_flash_erase(flash, addr_align, flash->erase_size);
		if (rc != 0)
			return rc;

		rc = spi_flash_write(flash, addr_align, flash->erase_size, buf);
		if (rc != 0)
			return rc;

		free(buf);

		// programm SECBOOT bit if required
		if (fustrap_orig & FUSTRAP_O_SECBOOT) {
			printf("%s(): program secure boot bit to FUSTRAP\n", __func__);
			rc = fuse_program_data(NPCM_FUSE_SA, 0, (u8*)(uintptr_t)&fustrap_orig, sizeof(fustrap_orig));
		} else {
			printf("%s(): secure boot bit is not set in the flash image, secure boot will not be enabled\n", __func__);
		}

		return rc;
	}
	// No fuse image was found in flash, continue with the normal boot flow

#endif

	return 0;
}
#endif

int last_stage_init(void)
{
	char value[32];
	struct udevice *dev = gd->cur_serial_dev;

	if (gd->ram_size > 0) {
		sprintf(value, "%lldM", (gd->ram_size / 0x100000));
		env_set("mem", value);
	}

	if (dev && (dev->seq_ >= 0)) {
		void *addr;
		addr = dev_read_addr_ptr(dev);

		if (addr) {
			sprintf(value, "uart8250,mmio32,0x%x", (u32)(uintptr_t)addr);
			env_set("earlycon", value);
		}
		sprintf(value, "ttyS%d,115200n8", dev->seq_);
		env_set("console", value);

	}
#ifdef SECURE_BOOT
	if (is_security_enabled()) {

		int rc = check_nist_version();
		if (rc != 0)
			return rc;
	} else {
		// OTP can be programmed only in basic mode
		int rc = secure_boot_configuration();
		if (rc != 0)
			return rc;
	}
#endif
	return 0;
}
#endif
