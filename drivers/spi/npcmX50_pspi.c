/*
 *  Copyright (c) 2017 Nuvoton Technology Corp.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <dm.h>
#include <spi.h>
#include <clk.h>
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gcr.h>
#include <asm/arch/pspi.h>
#include <asm/arch/spi_flash.h>
#include <asm/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

struct npcmX50_pspi_platdata {
	phys_addr_t regs;
	s32 frequency;
	u32 dev_num;
	struct gpio_desc cs_gpio;
};

struct npcmX50_pspi_priv {
	struct npcmX50_pspi_regs *pspi_regs;
	struct clk pspi_clk;
	u32 cs;
	enum pspi_dev pspi_dev_num;
};

static void gcr_mux_pspi(enum pspi_dev dev_num)
{
#if defined (CONFIG_TARGET_ARBEL)
	struct npcm850_gcr *gcr = (struct npcm850_gcr *)npcm850_get_base_gcr();
	
	writel(readl(&gcr->mfsel3) | (0x1 << MFSEL3_PSPISEL),
				&gcr->mfsel3);
				
#elif defined (CONFIG_TARGET_POLEG)
	struct npcm750_gcr *gcr = (struct npcm750_gcr *)npcm750_get_base_gcr();

	switch (dev_num) {
	case PSPI1_DEV:
	default:
		writel((readl(&gcr->mfsel3) & ~(0x3 << MFSEL3_PSPI1SEL))
				| (0x2 << MFSEL3_PSPI1SEL), &gcr->mfsel3);
		break;
	case PSPI2_DEV:
		writel(readl(&gcr->mfsel3) | (0x1 << MFSEL3_PSPI2SEL),
				&gcr->mfsel3);
		break;
	}
#endif
}

static void spi_cs_activate(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct npcmX50_pspi_platdata *plat = dev_get_platdata(bus);

	dm_gpio_set_value(&plat->cs_gpio, 0);

	return;
}

static void spi_cs_deactivate(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct npcmX50_pspi_platdata *plat = dev_get_platdata(bus);

	dm_gpio_set_value(&plat->cs_gpio, 1);

	return;
}


static int npcmX50_pspi_claim_bus(struct udevice *dev)
{
	return 0;
}

static int npcmX50_pspi_release_bus(struct udevice *dev)
{
	return 0;
}

static int npcmX50_pspi_xfer(struct udevice *dev, unsigned int bitlen,
			const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct npcmX50_pspi_priv *priv = dev_get_priv(bus);
	struct npcmX50_pspi_regs *regs = priv->pspi_regs;
	unsigned int bytes = bitlen / 8;
	const unsigned char *rx = dout;
	unsigned char *tx = din;
	int i;

	/* Cleaning junk data in the buffer */
	while (readb(&regs->pspi_stat) & (0x1 << PSPI_STAT_RBF))
		readb(&regs->pspi_data);

	if (flags & SPI_XFER_BEGIN)
		/* Setting chip select low to start transaction */
		spi_cs_activate(dev);

	/* Writing and reading the data */
	for (i = 0; i < bytes; i++) {

		/* Making sure we can write */
		while (readb(&regs->pspi_stat) & (0x1 << PSPI_STAT_BSY));

		if (rx)
			writeb(*rx++, &regs->pspi_data);
		else
			writeb(0, &regs->pspi_data);

		/* Wait till write completed */
		while (readb(&regs->pspi_stat) & (0x1 << PSPI_STAT_BSY));

		/* Waiting till reading is finished*/
		while (!(readb(&regs->pspi_stat) & (0x1 << PSPI_STAT_RBF)));

		if (tx)
			*tx++ = readb(&regs->pspi_data);
	}

	if (flags & SPI_XFER_END) {
		while (readb(&regs->pspi_stat) & (0x1 << PSPI_STAT_BSY));

		spi_cs_deactivate(dev);
	}

	return 0;
}

static int npcmX50_pspi_set_speed(struct udevice *bus, uint speed)
{
#if 0
	struct npcmX50_pspi_priv *priv = dev_get_priv(bus);

	int divisor;
	ulong apb_clock;

	apb_clock = clk_get_rate(&priv->pspi_clk);
	if (!apb_clock)
		return -EINVAL;

	/* Disabling the module for configuration */
	writew(readw(&priv->pspi_regs->pspi_ctl1) & ~(0x1 << PSPI_CTL1_SPIEN),
		&priv->pspi_regs->pspi_ctl1);

	/* Calculating divisor */
	divisor = (apb_clock / (2 * speed)) - 1;
	
	printf("%s: apb_clock=%lu speed=%d divisor=%d\n", __func__,apb_clock, speed, divisor);

	/* If requested clock frequency is to big we return ERROR */
	if (divisor <= 0)
		return -EINVAL;

	/* Setting the divisor */
	writew((readw(&priv->pspi_regs->pspi_ctl1) &
				~(0x7f << PSPI_CTL1_SCDV6_0)) |
			(divisor << PSPI_CTL1_SCDV6_0),
			&priv->pspi_regs->pspi_ctl1);

	/* Enabling the PSPI module */
	writew(readw(&priv->pspi_regs->pspi_ctl1) | (0x1 << PSPI_CTL1_SPIEN),
			&priv->pspi_regs->pspi_ctl1);
#endif
	return 0;
}

static int npcmX50_pspi_set_mode(struct udevice *bus, uint mode)
{
	struct npcmX50_pspi_priv *priv = dev_get_priv(bus);
	unsigned short pspi_mode;

	/* Disabling the module for configuration */
	writew(readw(&priv->pspi_regs->pspi_ctl1) & ~(0x1 << PSPI_CTL1_SPIEN),
			&priv->pspi_regs->pspi_ctl1);

	switch (mode & (SPI_CPOL | SPI_CPHA)) {
	case SPI_MODE_0:
		pspi_mode = 0;
		break;
	case SPI_MODE_1:
		pspi_mode = (0x1 << PSPI_CTL1_SCIDL);
		break;
	case SPI_MODE_2:
		pspi_mode = (0x1 << PSPI_CTL1_SCM);
		break;
	case SPI_MODE_3:
		pspi_mode = (0x1 << PSPI_CTL1_SCIDL) | (0x1 << PSPI_CTL1_SCM);
		break;
	default:
		break;
	}

	/* Set Mode and Enable the PSPI module */
	writew(readw(&priv->pspi_regs->pspi_ctl1) | (0x1 << PSPI_CTL1_SPIEN) | (pspi_mode),
			&priv->pspi_regs->pspi_ctl1);

	return 0;
}

static int npcmX50_spi_ofdata_to_platdata(struct udevice *bus)
{
	struct npcmX50_pspi_platdata *plat = dev_get_platdata(bus);
	const void *blob = gd->fdt_blob;
	int node = dev_of_offset(bus);

	plat->regs = (phys_addr_t)dev_read_addr_ptr(bus);
	/* Use 500KHz as a suitable default */
	plat->frequency = fdtdec_get_int(blob, node, "spi-max-frequency",
			500000);

	plat->dev_num = fdtdec_get_int(blob, node, "index", 0);

	return 0;
}

static int npcmX50_pspi_probe(struct udevice *bus)
{
	struct npcmX50_pspi_platdata *plat = dev_get_platdata(bus);
	struct npcmX50_pspi_priv *priv = dev_get_priv(bus);
	int node = dev_of_offset(bus);
	int ret;

	ret = clk_get_by_index(bus, 0, &priv->pspi_clk);
	if (ret < 0) {
		printf("%s:Probe failed: Failed to get clk!\n", __func__);
		return ret;
	}

	priv->pspi_dev_num = (enum pspi_dev)plat->dev_num;
	priv->pspi_regs = (struct npcmX50_pspi_regs *)plat->regs;
	
	gcr_mux_pspi(priv->pspi_dev_num);

	gpio_request_by_name_nodev(offset_to_ofnode(node), "cs-gpios", 0,
				&plat->cs_gpio, GPIOD_IS_OUT);

	return 0;
}


static const struct dm_spi_ops npcmX50_pspi_ops = {
	.claim_bus      = npcmX50_pspi_claim_bus,
	.release_bus    = npcmX50_pspi_release_bus,
	.xfer           = npcmX50_pspi_xfer,
	.set_speed      = npcmX50_pspi_set_speed,
	.set_mode       = npcmX50_pspi_set_mode,
	/*
	 * cs_info is not needed, since we require all chip selects to be
	 * in the device tree explicitly
	 */
};

static const struct udevice_id npcmX50_pspi_ids[] = {
	{ .compatible = "nuvoton,npcmX50-pspi"},
	{ }
};

U_BOOT_DRIVER(npcmX50_pspi) = {
	.name   = "npcmX50_pspi",
	.id     = UCLASS_SPI,
	.of_match = npcmX50_pspi_ids,
	.ops    = &npcmX50_pspi_ops,
	.ofdata_to_platdata = npcmX50_spi_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct npcmX50_pspi_platdata),
	.priv_auto_alloc_size = sizeof(struct npcmX50_pspi_priv),
	.probe  = npcmX50_pspi_probe,
};
