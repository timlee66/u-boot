// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022 Nuvoton Technology Corp.
 */

#include <dm.h>
#include <errno.h>
#include <regmap.h>
#include <syscon.h>
#include <asm/io.h>
#include <dm/device_compat.h>
#include <dm/pinctrl.h>
#include <linux/bitfield.h>
#include <asm/arch/rst.h>

/* GCR register offsets */
#define WD0RCR		0x38
#define WD1RCR		0x3C
#define WD2RCR		0x40
#define SWRSTC1		0x44
#define SWRSTC2		0x48
#define SWRSTC3		0x4C
#define SWRSTC4		0x50
#define CORSTC		0x5C
#define FLOCKR1		0x74
#define INTCR4		0xC0
#define I2CSEGSEL	0xE0
#define MFSEL1		0x260
#define MFSEL2		0x264
#define MFSEL3		0x268
#define MFSEL4		0x26C
#define MFSEL5		0x270
#define MFSEL6		0x274
#define MFSEL7		0x278

/* GPIO register offsets */
#define GPIO_POL	0x08 /* Polarity */
#define GPIO_DOUT	0x0c /* Data OUT */
#define GPIO_OTYP	0x14 /* Output Type */
#define GPIO_PU		0x1c /* Pull-up */
#define GPIO_PD		0x20 /* Pull-down */
#define GPIO_DBNC	0x24 /* Debounce */
#define GPIO_EVEN	0x40 /* Event Enable */
#define GPIO_EVST	0x4c /* Event Status */
#define GPIO_IEM	0x58 /* Input Enable */
#define GPIO_OSRC	0x5c /* Output Slew-Rate Control */
#define GPIO_ODSC	0x60 /* Output Drive Strength Control */
#define GPIO_OES	0x70 /* Output Enable Set */
#define GPIO_OEC	0x74 /* Output Enable Clear */

#define NPCM8XX_GPIO_PER_BANK	32
#define GPIOX_OFFSET	16

/* The lists contain alternate GPIO pins of the function */
/* Serial Interfaces */
static const unsigned int hsi1a_pins[] = { 43, 63 };
static const unsigned int hsi1b_pins[] = { 44, 62 };
static const unsigned int hsi1c_pins[] = { 45, 46, 47, 61 };
static const unsigned int hsi2a_pins[] = { 48, 49 };
static const unsigned int hsi2b_pins[] = { 50, 51 };
static const unsigned int hsi2c_pins[] = { 52, 53, 54, 55 };
static const unsigned int bmcuart0a_pins[] = { 41, 42 };
static const unsigned int bmcuart0b_pins[] = { 48, 49 };
static const unsigned int bmcuart1_pins[] = { 43, 63 };
static const unsigned int nbu1crts_pins[] = { 44, 62 };
static const unsigned int bu2_pins[] = { 96, 97};
static const unsigned int bu4_pins[] = { 54, 55 };
static const unsigned int bu4b_pins[] = { 98, 99 };
static const unsigned int bu5_pins[] = { 52, 53 };
static const unsigned int bu5b_pins[] = { 100, 101 };
static const unsigned int bu6_pins[] = { 50, 51 };

/* SPI/FIU/FLM */
static const unsigned int spi0cs1_pins[] = { 32 };
static const unsigned int spi1_pins[] = { 175, 176, 177, 203 };
static const unsigned int spi1cs1_pins[] = { 233 };
static const unsigned int spi1d23_pins[] = { 191, 192 };
static const unsigned int spi1cs2_pins[] = { 191 };
static const unsigned int spi1cs3_pins[] = { 192 };
static const unsigned int spi3_pins[] = { 183, 184, 185, 186 };
static const unsigned int spi3cs1_pins[] = { 187 };
static const unsigned int spi3quad_pins[] = { 188, 189 };
static const unsigned int spi3cs2_pins[] = { 188 };
static const unsigned int spi3cs3_pins[] = { 189 };
static const unsigned int spix_pins[] = { 224, 225, 226, 227, 229, 230 };
static const unsigned int spixcs1_pins[] = { 228 };
static const unsigned int pspi_pins[] = { 17, 18, 19 };
static const unsigned int fm2_pins[] = { 224, 225, 226, 227, 228, 229, 230 };
static const unsigned int fm1_pins[] = { 175, 176, 177, 203, 191, 192, 233 };
static const unsigned int fm0_pins[] = { 194, 195, 196, 202, 199, 198, 197 };

/* I2C */
static const unsigned int smb0_pins[]  = { 115, 114 };
static const unsigned int smb0b_pins[] = { 195, 194 };
static const unsigned int smb0c_pins[] = { 202, 196 };
static const unsigned int smb0d_pins[] = { 198, 199 };
static const unsigned int smb0den_pins[] = { 197 };
static const unsigned int smb1_pins[] = { 117, 116 };
static const unsigned int smb1b_pins[] = { 126, 127 };
static const unsigned int smb1c_pins[] = { 124, 125 };
static const unsigned int smb1d_pins[] = { 4, 5 };
static const unsigned int smb2_pins[] = { 119, 118 };
static const unsigned int smb2b_pins[] = { 122, 123 };
static const unsigned int smb2c_pins[] = { 120, 121 };
static const unsigned int smb2d_pins[] = { 6, 7 };
static const unsigned int smb3_pins[] = { 30, 31 };
static const unsigned int smb3b_pins[] = { 39, 40 };
static const unsigned int smb3c_pins[] = { 37, 38 };
static const unsigned int smb3d_pins[] = { 59, 60 };
static const unsigned int smb4_pins[] = { 28, 29 };
static const unsigned int smb4b_pins[] = { 18, 19 };
static const unsigned int smb4c_pins[] = { 20, 21 };
static const unsigned int smb4d_pins[] = { 22, 23 };
static const unsigned int smb5_pins[] = { 26, 27 };
static const unsigned int smb5b_pins[] = { 13, 12 };
static const unsigned int smb5c_pins[] = { 15, 14 };
static const unsigned int smb5d_pins[] = { 94, 93 };
static const unsigned int smb6_pins[] = { 172, 171 };
static const unsigned int smb6b_pins[] = { 2, 3 };
static const unsigned int smb6c_pins[] = { 0, 1 };
static const unsigned int smb6d_pins[] = { 10, 11 };
static const unsigned int smb7_pins[] = { 174, 173 };
static const unsigned int smb7b_pins[] = { 16, 141 };
static const unsigned int smb7c_pins[] = { 24, 25 };
static const unsigned int smb7d_pins[] = { 142, 143 };
static const unsigned int smb8_pins[] = { 129, 128 };
static const unsigned int smb9_pins[] = { 131, 130 };
static const unsigned int smb10_pins[] = { 133, 132 };
static const unsigned int smb11_pins[] = { 135, 134 };
static const unsigned int smb12_pins[] = { 221, 220 };
static const unsigned int smb13_pins[] = { 223, 222 };
static const unsigned int smb14_pins[] = { 22, 23 };
static const unsigned int smb14b_pins[] = { 32, 187 };
static const unsigned int smb15_pins[] = { 20, 21 };
static const unsigned int smb15b_pins[] = { 192, 191 };
static const unsigned int smb16_pins[] = { 10, 11 };
static const unsigned int smb16b_pins[] = { 218, 219 };
static const unsigned int smb17_pins[] = { 3, 2 };
static const unsigned int smb18_pins[] = { 0, 1 };
static const unsigned int smb19_pins[] = { 60, 59 };
static const unsigned int smb20_pins[] = { 234, 235 };
static const unsigned int smb21_pins[] = { 169, 170 };
static const unsigned int smb22_pins[] = { 40, 39 };
static const unsigned int smb23_pins[] = { 38, 37 };
static const unsigned int smb23b_pins[] = { 134, 134 };

/* I3C */
static const unsigned int i3c5_pins[] = { 106, 107 };
static const unsigned int i3c4_pins[] = { 33, 34 };
static const unsigned int i3c3_pins[] = { 246, 247 };
static const unsigned int i3c2_pins[] = { 244, 245 };
static const unsigned int i3c1_pins[] = { 242, 243 };
static const unsigned int i3c0_pins[] = { 240, 241 };

/* Fan/PWM */
static const unsigned int fanin0_pins[] = { 64 };
static const unsigned int fanin1_pins[] = { 65 };
static const unsigned int fanin2_pins[] = { 66 };
static const unsigned int fanin3_pins[] = { 67 };
static const unsigned int fanin4_pins[] = { 68 };
static const unsigned int fanin5_pins[] = { 69 };
static const unsigned int fanin6_pins[] = { 70 };
static const unsigned int fanin7_pins[] = { 71 };
static const unsigned int fanin8_pins[] = { 72 };
static const unsigned int fanin9_pins[] = { 73 };
static const unsigned int fanin10_pins[] = { 74 };
static const unsigned int fanin11_pins[] = { 75 };
static const unsigned int fanin12_pins[] = { 76 };
static const unsigned int fanin13_pins[] = { 77 };
static const unsigned int fanin14_pins[] = { 78 };
static const unsigned int fanin15_pins[] = { 79 };
static const unsigned int faninx_pins[] = { 175, 176, 177, 203 };
static const unsigned int pwm0_pins[] = { 80 };
static const unsigned int pwm1_pins[] = { 81 };
static const unsigned int pwm2_pins[] = { 82 };
static const unsigned int pwm3_pins[] = { 83 };
static const unsigned int pwm4_pins[] = { 144 };
static const unsigned int pwm5_pins[] = { 145 };
static const unsigned int pwm6_pins[] = { 146 };
static const unsigned int pwm7_pins[] = { 147 };
static const unsigned int pwm8_pins[] = { 220 };
static const unsigned int pwm9_pins[] = { 221 };
static const unsigned int pwm10_pins[] = { 234 };
static const unsigned int pwm11_pins[] = { 235 };

/* Network */
static const unsigned int rg1mdio_pins[] = { 108, 109 }; /* SGMII1 */
static const unsigned int rg2_pins[] = { 110, 111, 112, 113, 208, 209, 210, 211, 212,
	213, 214, 215 }; /* RGMII2 */
static const unsigned int rg2mdio_pins[] = { 216, 217 };
static const unsigned int rg2refck_pins[] = { 250 };
static const unsigned int r1_pins[] = { 178, 179, 180, 181, 182, 193, 201 }; /* RMII1 */
static const unsigned int r1err_pins[] = { 56 };
static const unsigned int r1oen_pins[] = { 56 };
static const unsigned int r1md_pins[] = { 57, 58 };
static const unsigned int r1en_pins[] = {  };
static const unsigned int r2_pins[] = { 84, 85, 86, 87, 88, 89, 200 }; /* RMII2 */
static const unsigned int r2md_pins[] = { 91, 92 };
static const unsigned int r2err_pins[] = { 90 };
static const unsigned int r2oen_pins[] = { 90 };
static const unsigned int r2en_pins[] = {  };
static const unsigned int rmii3_pins[] = { 110, 111, 209, 210, 211, 214, 215 };
static const unsigned int r3rxer_pins[] = { 212 };
static const unsigned int r3oen_pins[] = { 213 };
static const unsigned int r3en_pins[] = {  };

/* MMC */
static const unsigned int mmc_pins[] = { 152, 154, 156, 157, 158, 159 };
static const unsigned int mmc8_pins[] = { 148, 149, 150, 151 };
static const unsigned int mmcwp_pins[] = { 153 };
static const unsigned int mmccd_pins[] = { 155 };
static const unsigned int mmcrst_pins[] = { 155 };

/* JTAG */
static const unsigned int jtag2_pins[] = { 43, 44, 45, 46, 47 };
static const unsigned int j2j3_pins[] = { 44, 62, 45, 46 };
static const unsigned int jm1_pins[] = { 136, 137, 138, 139, 140 };
static const unsigned int jm2_pins[] = { 248 };

/* Serial I/O Expander*/
static const unsigned int iox1_pins[] = { 0, 1, 2, 3 };
static const unsigned int iox2_pins[] = { 4, 5, 6, 7 };
static const unsigned int ioxh_pins[] = { 10, 11, 24, 25 };

/* LPC/eSPI */
static const unsigned int lpc_pins[] = { 95, 161, 163, 164, 165, 166, 167 };
static const unsigned int espi_pins[] = { 95, 161, 163, 164, 165, 166, 167, 168 };
static const unsigned int clkrun_pins[] = { 162 };
static const unsigned int serirq_pins[] = { 168 };
static const unsigned int nprd_smi_pins[] = { 190 };
static const unsigned int scipme_pins[] = { 169 };
static const unsigned int sci_pins[] = { 170 };

/* VGA */
static const unsigned int vgadig_pins[] = { 102, 103, 104, 105 };
static const unsigned int gspi_pins[] = { 12, 13, 14, 15 };

/* PCIe */
static const unsigned int clkreq_pins[] = { 231 };

/* MISC */
static const unsigned int ga20kbc_pins[] = { 94, 93 };
static const unsigned int clkout_pins[] = { 160 };
static const unsigned int wdog1_pins[] = { 218 };
static const unsigned int wdog2_pins[] = { 219 };
static const unsigned int ddr_pins[] = { 110, 111, 112, 113, 208, 209, 210, 211, 212,
	213, 214, 215, 216, 217, 250 };

/* TIP/COP */
static const unsigned int tp_gpio7_pins[] = { 96 };
static const unsigned int tp_gpio6_pins[] = { 97 };
static const unsigned int tp_gpio5_pins[] = { 98 };
static const unsigned int tp_gpio4_pins[] = { 99 };
static const unsigned int tp_gpio3_pins[] = { 100 };
static const unsigned int tp_gpio2_pins[] = { 16 };
static const unsigned int tp_gpio1_pins[] = { 9 };
static const unsigned int tp_gpio0_pins[] = { 8 };
static const unsigned int tp_gpio5b_pins[] = { 58 };
static const unsigned int tp_gpio4b_pins[] = { 57 };
static const unsigned int tp_gpio2b_pins[] = { 101 };
static const unsigned int tp_gpio1b_pins[] = { 92 };
static const unsigned int tp_gpio0b_pins[] = { 91 };
static const unsigned int tp_uart_pins[] = { 50, 51 };
static const unsigned int tp_smb2_pins[] = { 24, 25 };
static const unsigned int tp_smb1_pins[] = { 142, 143 };
static const unsigned int tp_jtag3_pins[] = { 44, 62, 45, 46 };
static const unsigned int cp1gpio2c_pins[] = { 101 };
static const unsigned int cp1gpio3c_pins[] = { 100 };
static const unsigned int cp1gpio0b_pins[] = { 127 };
static const unsigned int cp1gpio1b_pins[] = { 126 };
static const unsigned int cp1gpio2b_pins[] = { 125 };
static const unsigned int cp1gpio3b_pins[] = { 124 };
static const unsigned int cp1gpio4b_pins[] = { 99 };
static const unsigned int cp1gpio5b_pins[] = { 98 };
static const unsigned int cp1gpio6b_pins[] = { 97 };
static const unsigned int cp1gpio7b_pins[] = { 96 };
static const unsigned int cp1gpio0_pins[] = {  };
static const unsigned int cp1gpio1_pins[] = {  };
static const unsigned int cp1gpio2_pins[] = {  };
static const unsigned int cp1gpio3_pins[] = {  };
static const unsigned int cp1gpio4_pins[] = {  };
static const unsigned int cp1gpio5_pins[] = { 17 };
static const unsigned int cp1gpio6_pins[] = { 91 };
static const unsigned int cp1gpio7_pins[] = { 92 };
static const unsigned int cp1utxd_pins[] = { 42 };
static const unsigned int cp1urxd_pins[] = { 41 };

/* General IO */
static const unsigned int gpi35_pins[] = { 35 };
static const unsigned int gpi36_pins[] = { 36 };
static const unsigned int gpio1836_pins[] = { 183, 184, 185, 186 };
static const unsigned int gpio1889_pins[] = { 188, 189 };
static const unsigned int gpio187_pins[] = { 187 };
static const unsigned int lkgpo0_pins[] = { 16 };
static const unsigned int lkgpo1_pins[] = { 8 };
static const unsigned int lkgpo2_pins[] = { 9 };
static const unsigned int hgpio0_pins[] = { 20 };
static const unsigned int hgpio1_pins[] = { 21 };
static const unsigned int hgpio2_pins[] = { 22 };
static const unsigned int hgpio3_pins[] = { 23 };
static const unsigned int hgpio4_pins[] = { 24 };
static const unsigned int hgpio5_pins[] = { 25 };
static const unsigned int hgpio6_pins[] = { 59 };
static const unsigned int hgpio7_pins[] = { 60 };
static char *gpio_func_name = "gpio";
static char **npcm8xx_funcs;
static int npcm8xx_num_funcs;

struct npcm8xx_pinctrl_priv {
	void __iomem *gpio_base;
	struct regmap *gcr_regmap;
	struct regmap *rst_regmap;
};

struct group_config {
	char *name;
	const unsigned int *pins;
	unsigned int npins;
	u32 reg; /* Register of setting func */
	u32 bit;
};

#define GRP(x, _reg, _bit) {			\
	.name = #x,				\
	.pins = x## _pins,			\
	.npins = ARRAY_SIZE(x## _pins),		\
	.reg = _reg,				\
	.bit = _bit,				\
	}
static const struct group_config npcm8xx_groups[] = {
	GRP(smb3, MFSEL1, 0),
	GRP(smb4, MFSEL1, 1),
	GRP(smb5, MFSEL1, 2),
	GRP(spi0cs1, MFSEL1, 3),
	GRP(hsi1c, MFSEL1, 4),
	GRP(hsi2c, MFSEL1, 5),
	GRP(smb0, MFSEL1, 6),
	GRP(smb1, MFSEL1, 7),
	GRP(smb2, MFSEL1, 8),
	GRP(bmcuart0a, MFSEL1, 9),
	GRP(hsi1a, MFSEL1, 10),
	GRP(hsi2a, MFSEL1, 11),
	GRP(r1err, MFSEL1, 12),
	GRP(r1md, MFSEL1, 13),
	GRP(r2, MFSEL1, 14),
	GRP(r2err, MFSEL1, 15),
	GRP(r2md, MFSEL1, 16),
	GRP(ga20kbc, MFSEL1, 17),
	GRP(clkout, MFSEL1, 21),
	GRP(sci, MFSEL1, 22),
	GRP(gspi, MFSEL1, 24),
	GRP(lpc, MFSEL1, 26),
	GRP(hsi1b, MFSEL1, 28),
	GRP(hsi2b, MFSEL1, 29),
	GRP(iox1, MFSEL1, 30),
	GRP(serirq, MFSEL1, 31),
	GRP(fanin0, MFSEL2, 0),
	GRP(fanin1, MFSEL2, 1),
	GRP(fanin2, MFSEL2, 2),
	GRP(fanin3, MFSEL2, 3),
	GRP(fanin4, MFSEL2, 4),
	GRP(fanin5, MFSEL2, 5),
	GRP(fanin6, MFSEL2, 6),
	GRP(fanin7, MFSEL2, 7),
	GRP(fanin8, MFSEL2, 8),
	GRP(fanin9, MFSEL2, 9),
	GRP(fanin10, MFSEL2, 10),
	GRP(fanin11, MFSEL2, 11),
	GRP(fanin12, MFSEL2, 12),
	GRP(fanin13, MFSEL2, 13),
	GRP(fanin14, MFSEL2, 14),
	GRP(fanin15, MFSEL2, 15),
	GRP(pwm0, MFSEL2, 16),
	GRP(pwm1, MFSEL2, 17),
	GRP(pwm2, MFSEL2, 18),
	GRP(pwm3, MFSEL2, 19),
	GRP(pwm4, MFSEL2, 20),
	GRP(pwm5, MFSEL2, 21),
	GRP(pwm6, MFSEL2, 22),
	GRP(pwm7, MFSEL2, 23),
	GRP(hgpio0, MFSEL2, 24),
	GRP(hgpio1, MFSEL2, 25),
	GRP(hgpio2, MFSEL2, 26),
	GRP(hgpio3, MFSEL2, 27),
	GRP(hgpio4, MFSEL2, 28),
	GRP(hgpio5, MFSEL2, 29),
	GRP(hgpio6, MFSEL2, 30),
	GRP(hgpio7, MFSEL2, 31),
	GRP(scipme, MFSEL3, 0),
	GRP(smb6, MFSEL3, 1),
	GRP(smb7, MFSEL3, 2),
	GRP(faninx, MFSEL3, 3),
	GRP(spi1, MFSEL3, 4),
	GRP(smb12, MFSEL3, 5),
	GRP(smb13, MFSEL3, 6),
	GRP(smb14, MFSEL3, 7),
	GRP(smb15, MFSEL3, 8),
	GRP(r1,	MFSEL3,	9),
	GRP(mmc, MFSEL3, 10),
	GRP(mmc8, MFSEL3, 11),
	GRP(pspi, MFSEL3, 13),
	GRP(iox2, MFSEL3, 14),
	GRP(clkrun, MFSEL3, 16),
	GRP(ioxh, MFSEL3, 18),
	GRP(wdog1, MFSEL3, 19),
	GRP(wdog2, MFSEL3, 20),
	GRP(i3c5, MFSEL3, 22),
	GRP(bmcuart1, MFSEL3, 24),
	GRP(mmccd, MFSEL3, 25),
	GRP(ddr, MFSEL3, 26),
	GRP(jtag2, MFSEL4, 0),
	GRP(bmcuart0b, MFSEL4, 1),
	GRP(mmcrst, MFSEL4, 6),
	GRP(espi, MFSEL4, 8),
	GRP(clkreq, MFSEL4, 9),
	GRP(smb8, MFSEL4, 11),
	GRP(smb9, MFSEL4, 12),
	GRP(smb10, MFSEL4, 13),
	GRP(smb11, MFSEL4, 14),
	GRP(spi3, MFSEL4, 16),
	GRP(spi3cs1, MFSEL4, 17),
	GRP(spi3cs2, MFSEL4, 18),
	GRP(spi3cs3, MFSEL4, 19),
	GRP(spi3quad, MFSEL4, 20),
	GRP(rg1mdio, MFSEL4, 21),
	GRP(bu2, MFSEL4, 22),
	GRP(rg2mdio, MFSEL4, 23),
	GRP(rg2, MFSEL4, 24),
	GRP(spix, MFSEL4, 27),
	GRP(spixcs1, MFSEL4, 28),
	GRP(spi1cs1, MFSEL5, 0),
	GRP(jm2, MFSEL5, 1),
	GRP(j2j3, MFSEL5, 2),
	GRP(spi1d23, MFSEL5, 3),
	GRP(spi1cs2, MFSEL5, 4),
	GRP(spi1cs3, MFSEL5, 5),
	GRP(bu6, MFSEL5, 6),
	GRP(bu5, MFSEL5, 7),
	GRP(bu4, MFSEL5, 8),
	GRP(r1oen, MFSEL5, 9),
	GRP(r2oen, MFSEL5, 10),
	GRP(rmii3, MFSEL5, 11),
	GRP(bu5b, MFSEL5, 12),
	GRP(bu4b, MFSEL5, 13),
	GRP(r3oen, MFSEL5, 14),
	GRP(jm1, MFSEL5, 15),
	GRP(gpi35, MFSEL5, 16),
	GRP(i3c0, MFSEL5, 17),
	GRP(gpi36, MFSEL5, 18),
	GRP(i3c1, MFSEL5, 19),
	GRP(tp_gpio4b, MFSEL5, 20),
	GRP(i3c2, MFSEL5, 21),
	GRP(tp_gpio5b, MFSEL5, 22),
	GRP(i3c3, MFSEL5, 23),
	GRP(smb16, MFSEL5, 24),
	GRP(smb17, MFSEL5, 25),
	GRP(smb18, MFSEL5, 26),
	GRP(smb19, MFSEL5, 27),
	GRP(smb20, MFSEL5, 28),
	GRP(smb21, MFSEL5, 29),
	GRP(smb22, MFSEL5, 30),
	GRP(smb23, MFSEL5, 31),
	GRP(smb23b, MFSEL6, 0),
	GRP(cp1utxd, MFSEL6, 1),
	GRP(cp1gpio0, MFSEL6, 2),
	GRP(cp1gpio1, MFSEL6, 3),
	GRP(cp1gpio2, MFSEL6, 4),
	GRP(cp1gpio3, MFSEL6, 5),
	GRP(cp1gpio4, MFSEL6, 6),
	GRP(cp1gpio5, MFSEL6, 7),
	GRP(cp1gpio6, MFSEL6, 8),
	GRP(cp1gpio7, MFSEL6, 9),
	GRP(i3c4, MFSEL6, 10),
	GRP(pwm8, MFSEL6, 11),
	GRP(pwm9, MFSEL6, 12),
	GRP(pwm10, MFSEL6, 13),
	GRP(pwm11, MFSEL6, 14),
	GRP(nbu1crts, MFSEL6, 15),
	GRP(fm0, MFSEL6, 16),
	GRP(fm1, MFSEL6, 17),
	GRP(fm2, MFSEL6, 18),
	GRP(gpio1836, MFSEL6, 19),
	GRP(cp1gpio0b, MFSEL6, 20),
	GRP(cp1gpio1b, MFSEL6, 21),
	GRP(cp1gpio2b, MFSEL6, 22),
	GRP(cp1gpio3b, MFSEL6, 23),
	GRP(cp1gpio7b, MFSEL6, 24),
	GRP(cp1gpio6b, MFSEL6, 25),
	GRP(cp1gpio5b, MFSEL6, 26),
	GRP(cp1gpio4b, MFSEL6, 27),
	GRP(cp1gpio3c, MFSEL6, 28),
	GRP(cp1gpio2c, MFSEL6, 29),
	GRP(r3rxer, MFSEL6, 30),
	GRP(cp1urxd, MFSEL6, 31),
	GRP(tp_gpio0, MFSEL7, 0),
	GRP(tp_gpio1, MFSEL7, 1),
	GRP(tp_gpio2, MFSEL7, 2),
	GRP(tp_gpio3, MFSEL7, 3),
	GRP(tp_gpio4, MFSEL7, 4),
	GRP(tp_gpio5, MFSEL7, 5),
	GRP(tp_gpio6, MFSEL7, 6),
	GRP(tp_gpio7, MFSEL7, 7),
	GRP(tp_gpio0b, MFSEL7, 8),
	GRP(tp_gpio1b, MFSEL7, 9),
	GRP(tp_gpio2b, MFSEL7, 10),
	GRP(tp_smb1, MFSEL7, 11),
	GRP(tp_uart, MFSEL7, 12),
	GRP(tp_jtag3, MFSEL7, 13),
	GRP(gpio187, MFSEL7, 24),
	GRP(gpio1889, MFSEL7, 25),
	GRP(smb14b, MFSEL7, 26),
	GRP(smb15b, MFSEL7, 27),
	GRP(tp_smb2, MFSEL7, 28),
	GRP(vgadig, MFSEL7, 29),
	GRP(smb16b, MFSEL7, 30),
	GRP(smb0b, I2CSEGSEL, 0),
	GRP(smb0c, I2CSEGSEL, 1),
	GRP(smb0d, I2CSEGSEL, 2),
	GRP(smb1b, I2CSEGSEL, 5),
	GRP(smb1c, I2CSEGSEL, 6),
	GRP(smb1d, I2CSEGSEL, 7),
	GRP(smb2b, I2CSEGSEL, 8),
	GRP(smb2c, I2CSEGSEL, 9),
	GRP(smb2d, I2CSEGSEL, 10),
	GRP(smb3b, I2CSEGSEL, 11),
	GRP(smb3c, I2CSEGSEL, 12),
	GRP(smb3d, I2CSEGSEL, 13),
	GRP(smb4b, I2CSEGSEL, 14),
	GRP(smb4c, I2CSEGSEL, 15),
	GRP(smb4d, I2CSEGSEL, 16),
	GRP(smb5b, I2CSEGSEL, 19),
	GRP(smb5c, I2CSEGSEL, 20),
	GRP(smb5d, I2CSEGSEL, 21),
	GRP(smb0den, I2CSEGSEL, 22),
	GRP(smb6b, I2CSEGSEL, 24),
	GRP(smb6c, I2CSEGSEL, 25),
	GRP(smb6d, I2CSEGSEL, 26),
	GRP(smb7b, I2CSEGSEL, 27),
	GRP(smb7c, I2CSEGSEL, 28),
	GRP(smb7d, I2CSEGSEL, 29),
	GRP(lkgpo0, FLOCKR1, 0),
	GRP(lkgpo1, FLOCKR1, 4),
	GRP(lkgpo2, FLOCKR1, 8),
	GRP(nprd_smi, FLOCKR1, 20),
	GRP(mmcwp, FLOCKR1, 24),
	GRP(rg2refck, INTCR4, 6),
	GRP(r1en, INTCR4, 12),
	GRP(r2en, INTCR4, 13),
	GRP(r3en, INTCR4, 14),
};

/* Pin flags */
#define SLEW		BIT(0) /* Has Slew Control */
#define DSLO_MASK	GENMASK(11, 8)  /* Drive strength */
#define DSHI_MASK	GENMASK(15, 12)
#define DS(lo, hi)	(((lo) << 8) | ((hi) << 12))
#define DSLO(x)		FIELD_GET(DSLO_MASK, x) /* Low DS value */
#define DSHI(x)		FIELD_GET(DSHI_MASK, x) /* High DS value */

#define MAX_ALT_FUNCS	5 /* Max alternate functions */
struct pin_info {
	int gpio_num;
	char *name;
	char *alt_func[MAX_ALT_FUNCS];
	int num_funcs;
	u32 flags;
};

static const struct pin_info npcm8xx_pins[] = {
	{0, "gpio0", {"iox1", "smb6c", "smb18"}, 3, SLEW},
	{1, "gpio1", {"iox1", "smb6c", "smb18"}, 3, SLEW},
	{2, "gpio2", {"iox1", "smb6b", "smb17"}, 3, SLEW},
	{3, "gpio3", {"iox1", "smb6b", "smb17"}, 3, SLEW},
	{4, "gpio4", {"iox2", "smb1d"}, 2, SLEW},
	{5, "gpio5", {"iox2", "smb1d"}, 2, SLEW},
	{6, "gpio6", {"iox2", "smb2d"}, 2, SLEW},
	{7, "gpio7", {"iox2", "smb2d"}, 2, SLEW},
	{8, "gpio8", {"lkgpo1", "tp_gpio0b"}, 2, DS(8, 12)},
	{9, "gpio9", {"lkgpo2", "tp_gpio1b"}, 2, DS(8, 12)},
	{10, "gpio10", {"ioxh", "smb6d", "smb16"}, 3, SLEW},
	{11, "gpio11", {"ioxh", "smb6d", "smb16"}, 3, SLEW},
	{12, "gpio12", {"gspi", "smb5d"}, 2, SLEW},
	{13, "gpio13", {"gspi", "smb5d"}, 2, SLEW},
	{14, "gpio14", {"gspi", "smb5c"}, 2, SLEW},
	{15, "gpio15", {"gspi", "smb5c"}, 2, SLEW},
	{16, "gpio16", {"lkgpo0", "smb7b", "tp_gpio2b"}, 3, SLEW},
	{17, "gpio17", {"pspi", "cp1gpio5"}, 2, SLEW},
	{18, "gpio18", {"pspi", "smb4b"}, 2, SLEW},
	{19, "gpio19", {"pspi", "smb4b"}, 2, SLEW},
	{20, "gpio20", {"hgpio0", "smb15", "smb4c"}, 3, SLEW},
	{21, "gpio21", {"hgpio1", "smb15", "smb4c"}, 3, SLEW},
	{22, "gpio22", {"hgpio2", "smb14", "smb4d"}, 3, SLEW},
	{23, "gpio23", {"hgpio3", "smb14", "smb4d"}, 3, SLEW},
	{24, "gpio24", {"hgpio4", "ioxh", "smb7c", "tp_smb2"}, 4, SLEW},
	{25, "gpio25", {"hgpio5", "ioxh", "smb7c"}, 3, SLEW},
	{26, "gpio26", {"smb5"}, 1, 0},
	{27, "gpio27", {"smb5"}, 1, 0},
	{28, "gpio28", {"smb4"}, 1, 0},
	{29, "gpio29", {"smb4"}, 1, 0},
	{30, "gpio30", {"smb3"}, 1, 0},
	{31, "gpio31", {"smb3"}, 1, 0},
	{32, "gpio32", {"smb14b", "spi0cs1"}, 2, SLEW},
	{33, "gpio33", {"i3c4"}, 1, SLEW},
	{34, "gpio34", {"i3c4"}, 1, SLEW},
	{35, "gpio35", {"gpi35"}, 1, 0},
	{36, "gpio36", {"gpi36"}, 1, 0},
	{37, "gpio37", {"smb3c", "smb23"}, 2, SLEW},
	{38, "gpio38", {"smb3c", "smb23"}, 2, SLEW},
	{39, "gpio39", {"smb3b", "smb22"}, 2, SLEW},
	{40, "gpio40", {"smb3b", "smb22"}, 2, SLEW},
	{41, "gpio41", {"bmcuart0a", "cp1urxd"}, 2, 0},
	{42, "gpio42", {"bmcuart0a", "cp1utxd"}, 2, DS(2, 4)},
	{43, "gpio43", {"hsi1a", "bmcuart1"}, 2, 0},
	{44, "gpio44", {"hsi1b", "nbu1crts", "jtag2", "tp_jtag3", "j2j3"}, 5, 0},
	{45, "gpio45", {"hsi1c", "jtag2", "j2j3", "tp_jtag3"}, 4, DS(2, 8)},
	{46, "gpio46", {"hsi1c", "jtag2", "j2j3", "tp_jtag3"}, 4, 0},
	{47, "gpio47", {"hsi1c",}, 1, DS(2, 8)},
	{48, "gpio48", {"hsi2a", "bmcuart0b"}, 2, 0},
	{49, "gpio49", {"hsi2a", "bmcuart0b"}, 2, 0},
	{50, "gpio50", {"hsi2b", "bu6", "tp_uart"}, 3, 0},
	{51, "gpio51", {"hsi2b", "bu6", "tp_uart"}, 3, 0},
	{52, "gpio52", {"hsi2c", "bu5"}, 2, 0},
	{53, "gpio53", {"hsi2c", "bu5"}, 2, 0},
	{54, "gpio54", {"hsi2c", "bu4"}, 2, 0},
	{55, "gpio55", {"hsi2c", "bu4"}, 2, 0},
	{56, "gpio56", {"r1err", "r1oen"}, 2, 0},
	{57, "gpio57", {"r1md", "tp_gpio4b"}, 2, DS(2, 4)},
	{58, "gpio58", {"r1md", "tp_gpio5b"}, 2, DS(2, 4)},
	{59, "gpio59", {"hgpio6", "smb3d", "smb19"}, 3, 0},
	{60, "gpio60", {"hgpio7", "smb3d", "smb19"}, 3, 0},
	{61, "gpio61", {"hsi1c"}, 1, 0},
	{62, "gpio62", {"hsi1b", "jtag2", "j2j3", "nbu1crts", "tp_jtag3"}, 5, 0},
	{63, "gpio63", {"hsi1a", "bmcuart1"}, 2, 0},
	{64, "gpio64", {"fanin0"}, 1, 0},
	{65, "gpio65", {"fanin1"}, 1, 0},
	{66, "gpio66", {"fanin2"}, 1, 0},
	{67, "gpio67", {"fanin3"}, 1, 0},
	{68, "gpio68", {"fanin4"}, 1, 0},
	{69, "gpio69", {"fanin5"}, 1, 0},
	{70, "gpio70", {"fanin6"}, 1, 0},
	{71, "gpio71", {"fanin7"}, 1, 0},
	{72, "gpio72", {"fanin8"}, 1, 0},
	{73, "gpio73", {"fanin9"}, 1, 0},
	{74, "gpio74", {"fanin10"}, 1, 0},
	{75, "gpio75", {"fanin11"}, 1, 0},
	{76, "gpio76", {"fanin12"}, 1, 0},
	{77, "gpio77", {"fanin13"}, 1, 0},
	{78, "gpio78", {"fanin14"}, 1, 0},
	{79, "gpio79", {"fanin15"}, 1, 0},
	{80, "gpio80", {"pwm0"}, 1, DS(4, 8)},
	{81, "gpio81", {"pwm1"}, 1, DS(4, 8)},
	{82, "gpio82", {"pwm2"}, 1, DS(4, 8)},
	{83, "gpio83", {"pwm3"}, 1, DS(4, 8)},
	{84, "gpio84", {"r2"}, 1, DS(4, 8) | SLEW},
	{85, "gpio85", {"r2"}, 1, DS(4, 8) | SLEW},
	{86, "gpio86", {"r2"}, 1, DS(4, 8) | SLEW},
	{87, "gpio87", {"r2"}, 1, 0},
	{88, "gpio88", {"r2"}, 1, 0},
	{89, "gpio89", {"r2"}, 1, 0},
	{90, "gpio90", {"r2err", "r2oen"}, 2, 0},
	{91, "gpio91", {"r2md", "cp1gpio6", "tp_gpio0"}, 3, DS(2, 4)},
	{92, "gpio92", {"r2md", "cp1gpio7", "tp_gpio1"}, 3, DS(2, 4)},
	{93, "gpio93", {"ga20kbc", "smb5d"}, 2, 0},
	{94, "gpio94", {"ga20kbc", "smb5d"}, 2, 0},
	{95, "gpio95", {"lpc", "espi"}, 2, 0},
	{96, "gpio96", {"cp1gpio7b", "bu2", "tp_gpio7"}, 3, SLEW},
	{97, "gpio97", {"cp1gpio6b", "bu2", "tp_gpio6"}, 3, SLEW},
	{98, "gpio98", {"bu4b", "cp1gpio5b", "tp_gpio5"}, 3, SLEW},
	{99, "gpio99", {"bu4b", "cp1gpio4b", "tp_gpio4"}, 3, SLEW},
	{100, "gpio100", {"bu5b", "cp1gpio3c", "tp_gpio3"}, 3, SLEW},
	{101, "gpio101", {"bu5b", "cp1gpio2c", "tp_gpio2"}, 3, SLEW},
	{102, "gpio102", {"vgadig"}, 1, DS(4, 8)},
	{103, "gpio103", {"vgadig"}, 1, DS(4, 8)},
	{104, "gpio104", {"vgadig"}, 1, 0},
	{105, "gpio105", {"vgadig"}, 1, 0},
	{106, "gpio106", {"i3c5"}, 1, SLEW},
	{107, "gpio107", {"i3c5"}, 1, SLEW},
	{108, "gpio108", {"rg1mdio"}, 1, 0},
	{109, "gpio109", {"rg1mdio"}, 1, 0},
	{110, "gpio110", {"rg2", "ddr", "rmii3"}, 3, 0},
	{111, "gpio111", {"rg2", "ddr", "rmii3"}, 3, 0},
	{112, "gpio112", {"rg2", "ddr"}, 2, 0},
	{113, "gpio113", {"rg2", "ddr"}, 2, 0},
	{114, "gpio114", {"smb0"}, 1, 0},
	{115, "gpio115", {"smb0"}, 1, 0},
	{116, "gpio116", {"smb1"}, 1, 0},
	{117, "gpio117", {"smb1"}, 1, 0},
	{118, "gpio118", {"smb2"}, 1, 0},
	{119, "gpio119", {"smb2"}, 1, 0},
	{120, "gpio120", {"smb2c"}, 1, SLEW},
	{121, "gpio121", {"smb2c"}, 1, SLEW},
	{122, "gpio122", {"smb2b"}, 1, SLEW},
	{123, "gpio123", {"smb2b"}, 1, SLEW},
	{124, "gpio124", {"smb1c", "cp1gpio3b"}, 2, SLEW},
	{125, "gpio125", {"smb1c", "cp1gpio2b"}, 2, SLEW},
	{126, "gpio126", {"smb1b", "cp1gpio1b"}, 2, SLEW},
	{127, "gpio127", {"smb1b", "cp1gpio0b"}, 2, SLEW},
	{128, "gpio128", {"smb8"}, 1, 0},
	{129, "gpio129", {"smb8"}, 1, 0},
	{130, "gpio130", {"smb9"}, 1, 0},
	{131, "gpio131", {"smb9"}, 1, 0},
	{132, "gpio132", {"smb10"}, 1, 0},
	{133, "gpio133", {"smb10"}, 1, 0},
	{134, "gpio134", {"smb11", "smb23b"}, 2, 0},
	{135, "gpio135", {"smb11", "smb23b"}, 2, 0},
	{136, "gpio136", {"jm1"}, 1, SLEW},
	{137, "gpio137", {"jm1"}, 1, SLEW},
	{138, "gpio138", {"jm1"}, 1, SLEW},
	{139, "gpio139", {"jm1"}, 1, SLEW},
	{140, "gpio140", {"jm1"}, 1, SLEW},
	{141, "gpio141", {"smb7b"}, 1, 0},
	{142, "gpio142", {"smb7d", "tp_smb1"}, 2, SLEW},
	{143, "gpio143", {"smb7d", "tp_smb1"}, 2, SLEW},
	{144, "gpio144", {"pwm4"}, 1, DS(4, 8)},
	{145, "gpio145", {"pwm5"}, 1, DS(4, 8)},
	{146, "gpio146", {"pwm6"}, 1, DS(4, 8)},
	{147, "gpio147", {"pwm7"}, 1, DS(4, 8)},
	{148, "gpio148", {"mmc8"}, 1, DS(8, 12) | SLEW},
	{149, "gpio149", {"mmc8"}, 1, DS(8, 12) | SLEW},
	{150, "gpio150", {"mmc8"}, 1, DS(8, 12) | SLEW},
	{151, "gpio151", {"mmc8"}, 1, DS(8, 12) | SLEW},
	{152, "gpio152", {"mmc"}, 1, DS(8, 12) | SLEW},
	{153, "gpio153", {"mmcwp"}, 1, 0},
	{154, "gpio154", {"mmc"}, 1, DS(8, 12) | SLEW},
	{155, "gpio155", {"mmccd", "mmcrst"}, 2, 0},
	{156, "gpio156", {"mmc"}, 1, DS(8, 12) | SLEW},
	{157, "gpio157", {"mmc"}, 1, DS(8, 12) | SLEW},
	{158, "gpio158", {"mmc"}, 1, DS(8, 12) | SLEW},
	{159, "gpio159", {"mmc"}, 1, DS(8, 12) | SLEW},
	{160, "gpio160", {"clkout"}, 1, DS(8, 12) | SLEW},
	{161, "gpio161", {"espi", "lpc"}, 2, 0},
	{162, "gpio162", {"clkrun"}, 1, DS(8, 12)},
	{163, "gpio163", {"espi", "lpc"}, 2, 0},
	{164, "gpio164", {"espi", "lpc"}, 2, 0},
	{165, "gpio165", {"espi", "lpc"}, 2, 0},
	{166, "gpio166", {"espi", "lpc"}, 2, 0},
	{167, "gpio167", {"espi", "lpc"}, 2, 0},
	{168, "gpio168", {"espi", "serirq"}, 2, 0},
	{169, "gpio169", {"scipme", "smb21"}, 2, 0},
	{170, "gpio170", {"sci", "smb21"}, 2, 0},
	{171, "gpio171", {"smb6"}, 1, 0},
	{172, "gpio172", {"smb6"}, 1, 0},
	{173, "gpio173", {"smb7"}, 1, 0},
	{174, "gpio174", {"smb7"}, 1, 0},
	{175, "gpio175", {"spi1", "faninx", "fm1"}, 3, DS(8, 12)},
	{176, "gpio176", {"spi1", "faninx", "fm1"}, 3, DS(8, 12)},
	{177, "gpio177", {"spi1", "faninx", "fm1"}, 3, DS(8, 12)},
	{178, "gpio178", {"r1"}, 1, DS(8, 12) | SLEW},
	{179, "gpio179", {"r1"}, 1, DS(8, 12) | SLEW},
	{180, "gpio180", {"r1"}, 1, DS(8, 12) | SLEW},
	{181, "gpio181", {"r1"}, 1, 0},
	{182, "gpio182", {"r1"}, 1, 0},
	{183, "gpio183", {"spi3", "gpio1836"}, 2, DS(8, 12) | SLEW},
	{184, "gpio184", {"spi3", "gpio1836"}, 2, DS(8, 12) | SLEW},
	{185, "gpio185", {"spi3", "gpio1836"}, 2, DS(8, 12) | SLEW},
	{186, "gpio186", {"spi3", "gpio1836"}, 2, DS(8, 12) | SLEW},
	{187, "gpio187", {"spi3cs1", "smb14b", "gpio187"}, 3, SLEW},
	{188, "gpio188", {"spi3quad", "spi3cs2", "gpio1889"}, 3, DS(8, 12) | SLEW},
	{189, "gpio189", {"spi3quad", "spi3cs3", "gpio1889"}, 3, DS(8, 12) | SLEW},
	{190, "gpio190", {"nprd_smi"}, 1, DS(2, 4)},
	{191, "gpio191", {"spi1d23", "spi1cs2", "fm1", "smb15"}, 4, SLEW},
	{192, "gpio192", {"spi1d23", "spi1cs3", "fm1", "smb15"}, 4, SLEW},
	{193, "gpio193", {"r1"}, 1, 0},
	{194, "gpio194", {"smb0b", "fm0"}, 2, SLEW},
	{195, "gpio195", {"smb0b", "fm0"}, 2, SLEW},
	{196, "gpio196", {"smb0c", "fm0"}, 2, SLEW},
	{197, "gpio197", {"smb0den", "fm0"}, 2, SLEW},
	{198, "gpio198", {"smb0d", "fm0"}, 2, SLEW},
	{199, "gpio199", {"smb0d", "fm0"}, 2, SLEW},
	{200, "gpio200", {"r2"}, 1, 0},
	{201, "gpio201", {"r1"}, 1, 0},
	{202, "gpio202", {"smb0c", "fm0"}, 2, SLEW},
	{203, "gpio203", {"faninx", "spi1", "fm1"}, 3, DS(8, 12)},
	{208, "gpio208", {"rg2", "ddr"}, 2, SLEW},
	{209, "gpio209", {"rg2", "ddr", "rmii3"}, 3, SLEW},
	{210, "gpio210", {"rg2", "ddr", "rmii3"}, 3, DS(8, 12) | SLEW},
	{211, "gpio211", {"rg2", "ddr", "rmii3"}, 3, DS(8, 12) | SLEW},
	{212, "gpio212", {"rg2", "ddr", "r3rxer"}, 3, DS(8, 12) | SLEW},
	{213, "gpio213", {"rg2", "ddr", "r3oen"}, 3, DS(8, 12) | SLEW},
	{214, "gpio214", {"rg2", "ddr", "rmii3"}, 3, DS(8, 12) | SLEW},
	{215, "gpio215", {"rg2", "ddr", "rmii3"}, 3, DS(8, 12) | SLEW},
	{216, "gpio216", {"rg2mdio", "ddr"}, 2, DS(8, 12) | SLEW},
	{217, "gpio217", {"rg2mdio", "ddr"}, 2, DS(8, 12) | SLEW},
	{218, "gpio218", {"wdog1", "smb16"}, 2, SLEW},
	{219, "gpio219", {"wdog2", "smb16"}, 2, SLEW},
	{220, "gpio220", {"smb12", "pwm8"}, 2, SLEW},
	{221, "gpio221", {"smb12", "pwm9"}, 2, SLEW},
	{222, "gpio222", {"smb13"}, 1, SLEW},
	{223, "gpio223", {"smb13"}, 1, SLEW},
	{224, "gpio224", {"spix", "fm2"}, 2, DS(8, 12) | SLEW},
	{225, "gpio225", {"spix", "fm2"}, 2, DS(8, 12) | SLEW},
	{226, "gpio226", {"spix", "fm2"}, 2, DS(8, 12) | SLEW},
	{227, "gpio227", {"spix", "fm2"}, 2, DS(8, 12) | SLEW},
	{228, "gpio228", {"spixcs1", "fm2"}, 2, DS(8, 12) | SLEW},
	{229, "gpio229", {"spix", "fm2"}, 2, DS(8, 12) | SLEW},
	{230, "gpio230", {"spix", "fm2"}, 2, DS(8, 12) | SLEW},
	{231, "gpio231", {"clkreq"}, 1, DS(8, 12) | SLEW},
	{233, "gpio233", {"spi1cs1", "fm1"}, 2, 0},
	{234, "gpio234", {"pwm10", "smb20"}, 2, SLEW},
	{235, "gpio235", {"pwm11", "smb20"}, 2, SLEW},
	{240, "gpio240", {"i3c0"}, 2, SLEW},
	{241, "gpio241", {"i3c0"}, 2, SLEW},
	{242, "gpio242", {"i3c1"}, 2, SLEW},
	{243, "gpio243", {"i3c1"}, 2, SLEW},
	{244, "gpio244", {"i3c2"}, 2, SLEW},
	{245, "gpio245", {"i3c2"}, 2, SLEW},
	{246, "gpio246", {"i3c3"}, 2, SLEW},
	{247, "gpio247", {"i3c3"}, 2, SLEW},
	{250, "gpio250", {"ddr", "rg2refck"}, 2, DS(8, 12) | SLEW},
};

static const struct group_config *npcm8xx_group_get(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(npcm8xx_groups); i++) {
		if (!strcmp(npcm8xx_groups[i].name, name))
			return &npcm8xx_groups[i];
	}

	return NULL;
}

static int npcm8xx_get_pin_selector(int gpio)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(npcm8xx_pins); i++) {
		if (npcm8xx_pins[i].gpio_num == gpio)
			return i;
	}

	return -ENOENT;
}

static int npcm8xx_group_set_func(struct udevice *dev,
				  const struct group_config *group,
				  char *func)
{
	struct npcm8xx_pinctrl_priv *priv = dev_get_priv(dev);

	dev_dbg(dev, "set_func [grp %s][func %s]\n", group->name, func);
	if (strcmp(group->name, func))
		regmap_update_bits(priv->gcr_regmap, group->reg,
				   BIT(group->bit), 0);
	else
		regmap_update_bits(priv->gcr_regmap, group->reg,
				   BIT(group->bit), BIT(group->bit));

	return 0;
}

static int npcm8xx_pinmux_set(struct udevice *dev,
			      unsigned int pin_selector,
			      unsigned int func_selector)
{
	const struct pin_info *pin;
	char *func = npcm8xx_funcs[func_selector];
	const struct group_config *group;
	int i;

	pin = &npcm8xx_pins[pin_selector];
	dev_dbg(dev, "set_mux [pin %s][func %s]\n", pin->name, func);

	for (i = 0; i < pin->num_funcs; i++) {
		group = npcm8xx_group_get(pin->alt_func[i]);
		if (!group)
			break;
		npcm8xx_group_set_func(dev, group, func);
	}

	return 0;
}

static int npcm8xx_pinmux_group_set(struct udevice *dev,
				    unsigned int group_selector,
				    unsigned int func_selector)
{
	const struct group_config *group;
	int pin_selector;
	int i;

	dev_dbg(dev, "set_mux [grp %s][func %s]\n",
		npcm8xx_groups[group_selector].name,
		npcm8xx_funcs[func_selector]);
	group = &npcm8xx_groups[group_selector];

	if (group->npins == 0) {
		/* No alternate GPIO pins, just set the function */
		npcm8xx_group_set_func(dev, group,
				       npcm8xx_funcs[func_selector]);
		return 0;
	}

	for (i = 0; i < group->npins; i++) {
		pin_selector = npcm8xx_get_pin_selector(group->pins[i]);
		if (pin_selector < 0) {
			dev_dbg(dev, "invalid pin %d\n", group->pins[i]);
			return -EINVAL;
		}
		npcm8xx_pinmux_set(dev, pin_selector, func_selector);
	}

	return 0;
}

static int npcm8xx_get_pins_count(struct udevice *dev)
{
	return ARRAY_SIZE(npcm8xx_pins);
}

static const char *npcm8xx_get_pin_name(struct udevice *dev,
					unsigned int selector)
{
	return npcm8xx_pins[selector].name;
}

static int npcm8xx_get_groups_count(struct udevice *dev)
{
	return ARRAY_SIZE(npcm8xx_groups);
}

static const char *npcm8xx_get_group_name(struct udevice *dev,
					  unsigned int selector)
{
	return npcm8xx_groups[selector].name;
}

static int npcm8xx_get_functions_count(struct udevice *dev)
{
	return npcm8xx_num_funcs;
}

static const char *npcm8xx_get_function_name(struct udevice *dev,
					     unsigned int selector)
{
	return npcm8xx_funcs[selector];
}

#if CONFIG_IS_ENABLED(PINCONF)
#define PIN_CONFIG_PERSIST_STATE (PIN_CONFIG_END + 1)
#define PIN_CONFIG_POLARITY_STATE (PIN_CONFIG_END + 2)
#define PIN_CONFIG_EVENT_CLEAR (PIN_CONFIG_END + 3)

static const struct pinconf_param npcm8xx_conf_params[] = {
	{ "bias-disable", PIN_CONFIG_BIAS_DISABLE, 0 },
	{ "bias-pull-up", PIN_CONFIG_BIAS_PULL_UP, 1 },
	{ "bias-pull-down", PIN_CONFIG_BIAS_PULL_DOWN, 1 },
	{ "input-enable", PIN_CONFIG_INPUT_ENABLE, 1 },
	{ "output-enable", PIN_CONFIG_OUTPUT_ENABLE, 1 },
	{ "output-high", PIN_CONFIG_OUTPUT, 1, },
	{ "output-low", PIN_CONFIG_OUTPUT, 0, },
	{ "drive-open-drain", PIN_CONFIG_DRIVE_OPEN_DRAIN, 1 },
	{ "drive-push-pull", PIN_CONFIG_DRIVE_PUSH_PULL, 1 },
	{ "persist-enable", PIN_CONFIG_PERSIST_STATE, 1 },
	{ "persist-disable", PIN_CONFIG_PERSIST_STATE, 0 },
	{ "input-debounce", PIN_CONFIG_INPUT_DEBOUNCE, 0 },
	{ "active-high", PIN_CONFIG_POLARITY_STATE, 0 },
	{ "active-low", PIN_CONFIG_POLARITY_STATE, 1 },
	{ "drive-strength", PIN_CONFIG_DRIVE_STRENGTH, 0 },
	{ "slew-rate", PIN_CONFIG_SLEW_RATE, 0},
	{ "event-clear", PIN_CONFIG_EVENT_CLEAR, 0},
};

/* Support for retaining the state after soft reset */
static int npcm8xx_gpio_reset_persist(struct udevice *dev, unsigned int bank,
				      unsigned int enable)
{
	struct npcm8xx_pinctrl_priv *priv = dev_get_priv(dev);
	u8 offset = bank + GPIOX_OFFSET;

	dev_dbg(dev, "set gpio persist, bank %d, enable %d\n", bank, enable);

	if (enable) {
		regmap_update_bits(priv->rst_regmap, WD0RCR, BIT(offset), 0);
		regmap_update_bits(priv->rst_regmap, WD1RCR, BIT(offset), 0);
		regmap_update_bits(priv->rst_regmap, WD2RCR, BIT(offset), 0);
		regmap_update_bits(priv->rst_regmap, CORSTC, BIT(offset), 0);
	} else {
		regmap_update_bits(priv->rst_regmap, WD0RCR, BIT(offset),
				   BIT(offset));
		regmap_update_bits(priv->rst_regmap, WD1RCR, BIT(offset),
				   BIT(offset));
		regmap_update_bits(priv->rst_regmap, WD2RCR, BIT(offset),
				   BIT(offset));
		regmap_update_bits(priv->rst_regmap, CORSTC, BIT(offset),
				   BIT(offset));
	}

	return 0;
}

static bool is_gpio_persist(struct udevice *dev, unsigned int bank)
{
	struct npcm8xx_pinctrl_priv *priv = dev_get_priv(dev);
	u8 offset = bank + GPIOX_OFFSET;
	u32 val;
	int status;

	status = npcm_get_reset_status();
	dev_dbg(dev, "reset status: 0x%x\n", status);

	if (status & CORST)
		regmap_read(priv->rst_regmap, CORSTC, &val);
	else if (status & WD0RST)
		regmap_read(priv->rst_regmap, WD0RCR, &val);
	else if (status & WD1RST)
		regmap_read(priv->rst_regmap, WD1RCR, &val);
	else if (status & WD2RST)
		regmap_read(priv->rst_regmap, WD2RCR, &val);
	else
		return false;

	return !(val & BIT(offset));
}

static int npcm8xx_pinconf_set(struct udevice *dev, unsigned int selector,
			       unsigned int param, unsigned int arg)
{
	struct npcm8xx_pinctrl_priv *priv = dev_get_priv(dev);
	unsigned int pin = npcm8xx_pins[selector].gpio_num;
	unsigned int bank = pin / NPCM8XX_GPIO_PER_BANK;
	unsigned int gpio = (pin % NPCM8XX_GPIO_PER_BANK);
	void __iomem *base = priv->gpio_base + (0x1000 * bank);
	u32 flags = npcm8xx_pins[pin].flags;
	int ret = 0;

	dev_dbg(dev, "set_conf [pin %d][param 0x%x, arg 0x%x]\n",
		pin, param, arg);

	if (is_gpio_persist(dev, bank)) {
		dev_dbg(dev, "retain the state\n");
		return 0;
	}

	switch (param) {
	case PIN_CONFIG_BIAS_DISABLE:
		dev_dbg(dev, "set pin %d bias disable\n", pin);
		clrbits_le32(base + GPIO_PU, BIT(gpio));
		clrbits_le32(base + GPIO_PD, BIT(gpio));
		break;
	case PIN_CONFIG_BIAS_PULL_DOWN:
		dev_dbg(dev, "set pin %d bias pull down\n", pin);
		clrbits_le32(base + GPIO_PU, BIT(gpio));
		setbits_le32(base + GPIO_PD, BIT(gpio));
		break;
	case PIN_CONFIG_BIAS_PULL_UP:
		dev_dbg(dev, "set pin %d bias pull up\n", pin);
		setbits_le32(base + GPIO_PU, BIT(gpio));
		clrbits_le32(base + GPIO_PD, BIT(gpio));
		break;
	case PIN_CONFIG_INPUT_ENABLE:
		dev_dbg(dev, "set pin %d input enable\n", pin);
		setbits_le32(base + GPIO_OEC, BIT(gpio));
		setbits_le32(base + GPIO_IEM, BIT(gpio));
		break;
	case PIN_CONFIG_OUTPUT_ENABLE:
		dev_dbg(dev, "set pin %d output enable\n", pin);
		clrbits_le32(base + GPIO_IEM, BIT(gpio));
		setbits_le32(base + GPIO_OES, BIT(gpio));
	case PIN_CONFIG_OUTPUT:
		dev_dbg(dev, "set pin %d output %d\n", pin, arg);
		clrbits_le32(base + GPIO_IEM, BIT(gpio));
		setbits_le32(base + GPIO_OES, BIT(gpio));
		if (arg)
			setbits_le32(base + GPIO_DOUT, BIT(gpio));
		else
			clrbits_le32(base + GPIO_DOUT, BIT(gpio));
		break;
	case PIN_CONFIG_DRIVE_PUSH_PULL:
		dev_dbg(dev, "set pin %d push pull\n", pin);
		clrbits_le32(base + GPIO_OTYP, BIT(gpio));
		break;
	case PIN_CONFIG_DRIVE_OPEN_DRAIN:
		dev_dbg(dev, "set pin %d open drain\n", pin);
		setbits_le32(base + GPIO_OTYP, BIT(gpio));
		break;
	case PIN_CONFIG_INPUT_DEBOUNCE:
		dev_dbg(dev, "set pin %d input debounce\n", pin);
		setbits_le32(base + GPIO_DBNC, BIT(gpio));
		break;
	case PIN_CONFIG_POLARITY_STATE:
		dev_dbg(dev, "set pin %d active %d\n", pin, arg);
		if (arg)
			setbits_le32(base + GPIO_POL, BIT(gpio));
		else
			clrbits_le32(base + GPIO_POL, BIT(gpio));
		break;
	case PIN_CONFIG_DRIVE_STRENGTH:
		dev_dbg(dev, "set pin %d driver strength %d\n", pin, arg);
		if (DSLO(flags) == arg)
			clrbits_le32(base + GPIO_ODSC, BIT(gpio));
		else if (DSHI(flags) == arg)
			setbits_le32(base + GPIO_ODSC, BIT(gpio));
		else
			ret = -EOPNOTSUPP;
		break;
	case PIN_CONFIG_SLEW_RATE:
		dev_dbg(dev, "set pin %d slew rate %d\n", pin, arg);
		if (!(flags & SLEW)) {
			ret = -EOPNOTSUPP;
			break;
		}
		if (arg)
			setbits_le32(base + GPIO_OSRC, BIT(gpio));
		else
			clrbits_le32(base + GPIO_OSRC, BIT(gpio));
		break;
	case PIN_CONFIG_EVENT_CLEAR:
		dev_dbg(dev, "set pin %d event clear\n", pin);
		clrbits_le32(base + GPIO_EVEN, BIT(gpio));
		setbits_le32(base + GPIO_EVST, BIT(gpio));
		break;
	case  PIN_CONFIG_PERSIST_STATE:
		npcm8xx_gpio_reset_persist(dev, bank, arg);
		break;

	default:
		ret = -EOPNOTSUPP;
	}

	return ret;
}
#endif

static struct pinctrl_ops npcm8xx_pinctrl_ops = {
	.set_state	= pinctrl_generic_set_state,
	.get_pins_count = npcm8xx_get_pins_count,
	.get_pin_name = npcm8xx_get_pin_name,
	.get_groups_count = npcm8xx_get_groups_count,
	.get_group_name = npcm8xx_get_group_name,
	.get_functions_count = npcm8xx_get_functions_count,
	.get_function_name = npcm8xx_get_function_name,
	.pinmux_set = npcm8xx_pinmux_set,
	.pinmux_group_set = npcm8xx_pinmux_group_set,
#if CONFIG_IS_ENABLED(PINCONF)
	.pinconf_num_params = ARRAY_SIZE(npcm8xx_conf_params),
	.pinconf_params = npcm8xx_conf_params,
	.pinconf_set = npcm8xx_pinconf_set,
	.pinconf_group_set = npcm8xx_pinconf_set,
#endif
};

static int npcm8xx_pinctrl_probe(struct udevice *dev)
{
	struct npcm8xx_pinctrl_priv *priv = dev_get_priv(dev);
	int i;

	priv->gpio_base = dev_read_addr_ptr(dev);
	if (!priv->gpio_base)
		return -EINVAL;

	priv->gcr_regmap = syscon_regmap_lookup_by_phandle(dev, "syscon-gcr");
	if (IS_ERR(priv->gcr_regmap))
		return -EINVAL;

	priv->rst_regmap = syscon_regmap_lookup_by_phandle(dev, "syscon-rst");
	if (IS_ERR(priv->rst_regmap))
		return -EINVAL;

	/* initialize function names, all group functions + gpio function */
	npcm8xx_num_funcs = ARRAY_SIZE(npcm8xx_groups) + 1;
	npcm8xx_funcs = malloc(npcm8xx_num_funcs * sizeof(char *));
	if (!npcm8xx_funcs)
		return -ENOMEM;
	npcm8xx_funcs[0] = gpio_func_name;
	for (i = 1; i < npcm8xx_num_funcs; i++)
		npcm8xx_funcs[i] = npcm8xx_groups[i - 1].name;

	return 0;
}

static const struct udevice_id npcm8xx_pinctrl_ids[] = {
	{ .compatible = "nuvoton,npcm845-pinctrl" },
	{ }
};

U_BOOT_DRIVER(pinctrl_npcm8xx) = {
	.name = "nuvoton_npcm8xx_pinctrl",
	.id = UCLASS_PINCTRL,
	.of_match = npcm8xx_pinctrl_ids,
	.priv_auto = sizeof(struct npcm8xx_pinctrl_priv),
	.ops = &npcm8xx_pinctrl_ops,
	.probe = npcm8xx_pinctrl_probe,
};
