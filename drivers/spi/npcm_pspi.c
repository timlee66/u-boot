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
#include <asm/io.h>
#include <asm/arch/pspi.h>
#include <asm/gpio.h>
#include <linux/iopoll.h>

DECLARE_GLOBAL_DATA_PTR;

#define MAX_DIV	127
struct npcm_pspi_platdata {
	phys_addr_t regs;
	u32 max_hz;
	u32 dev_num;
	struct gpio_desc cs_gpio;
};

struct npcm_pspi_priv {
	struct npcm_pspi_regs *pspi_regs;
	struct clk pspi_clk;
	u32 cs;
	enum pspi_dev pspi_dev_num;
};

static void dump_regs(struct npcm_pspi_priv *priv)
{
	struct npcm_pspi_regs *regs = priv->pspi_regs;

	printf("pspi_stat=0x%02x\n", readb(&regs->pspi_stat));
	printf("pspi_ctl1=0x%04x\n", readw(&regs->pspi_ctl1));
}

static void spi_cs_activate(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct npcm_pspi_platdata *plat = dev_get_plat(bus);

	dm_gpio_set_value(&plat->cs_gpio, 0);

	return;
}

static void spi_cs_deactivate(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct npcm_pspi_platdata *plat = dev_get_plat(bus);

	dm_gpio_set_value(&plat->cs_gpio, 1);

	return;
}


static int npcm_pspi_claim_bus(struct udevice *dev)
{
	return 0;
}

static int npcm_pspi_release_bus(struct udevice *dev)
{
	return 0;
}

static int npcm_pspi_xfer(struct udevice *dev, unsigned int bitlen,
			const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct npcm_pspi_priv *priv = dev_get_priv(bus);
	struct npcm_pspi_regs *regs = priv->pspi_regs;
	unsigned int bytes = bitlen / 8;
	const unsigned char *tx = dout;
	unsigned char *rx = din;
	char tmp;
	u32 val;
	int i, ret = 0;

	/* Cleaning junk data in the buffer */
	if (readb(&regs->pspi_stat) & (0x1 << PSPI_STAT_RBF))
		readb(&regs->pspi_data);

	if (flags & SPI_XFER_BEGIN)
		/* Setting chip select low to start transaction */
		spi_cs_activate(dev);

	/* Writing and reading the data */
	for (i = 0; i < bytes; i++) {

		/* Making sure we can write */
		ret = readb_poll_timeout(&regs->pspi_stat, val,
				!(val & (0x1 << PSPI_STAT_BSY)), 1000000);
		if (ret == -ETIMEDOUT) {
			printf("%s: state is busy\n", __func__);
			dump_regs(priv);
			ret = -EBUSY;
			goto err;
		}

		if (tx)
			writeb(*tx++, &regs->pspi_data);
		else
			writeb(0, &regs->pspi_data);

		/* Wait till write completed */
		ret = readb_poll_timeout(&regs->pspi_stat, val,
				!(val & (0x1 << PSPI_STAT_BSY)), 1000000);
		if (ret == -ETIMEDOUT) {
			printf("%s: state is busy after write\n", __func__);
			dump_regs(priv);
			ret = -EBUSY;
			goto err;
		}

		/* Waiting till reading is finished*/
		ret = readb_poll_timeout(&regs->pspi_stat, val,
				(val & (0x1 << PSPI_STAT_RBF)), 1000000);
		if (ret == -ETIMEDOUT) {
			printf("%s: read buf is empty\n", __func__);
			dump_regs(priv);
			ret = -EBUSY;
			goto err;
		}

		tmp = readb(&regs->pspi_data);
		if (rx)
			*rx++ = tmp;
	}

	if (flags & SPI_XFER_END) {
		ret = readb_poll_timeout(&regs->pspi_stat, val,
				!(val & (0x1 << PSPI_STAT_BSY)), 1000000);
		if (ret == -ETIMEDOUT) {
			printf("%s: state is busy after xfer\n", __func__);
			dump_regs(priv);
			ret = -EBUSY;
		}

		spi_cs_deactivate(dev);
	}

	return ret;

err:
	spi_cs_deactivate(dev);

	return ret;
}

static int npcm_pspi_set_speed(struct udevice *bus, uint speed)
{
	struct npcm_pspi_priv *priv = dev_get_priv(bus);
	struct npcm_pspi_platdata *plat = dev_get_plat(bus);
	u32 divisor;
	u32 apb_clock;

	apb_clock = clk_get_rate(&priv->pspi_clk);
	if (!apb_clock)
		return -EINVAL;

	if (speed > plat->max_hz)
		speed = plat->max_hz;

	/* Disabling the module for configuration */
	writew(readw(&priv->pspi_regs->pspi_ctl1) & ~(0x1 << PSPI_CTL1_SPIEN),
		&priv->pspi_regs->pspi_ctl1);

	/* Calculating divisor */
	divisor = DIV_ROUND_CLOSEST(apb_clock, (2 * speed) - 1);
	if (divisor > MAX_DIV)
		divisor = MAX_DIV;

	debug("%s: apb_clock=%u speed=%d divisor=%u\n",
			__func__,	apb_clock, speed, divisor);

	/* If requested clock frequency is to big we return ERROR */
	if (divisor <= 0)
		return -EINVAL;

	/* Setting the divisor */
	writew((readw(&priv->pspi_regs->pspi_ctl1) &
				~(0x7f << PSPI_CTL1_SCDV6_0)) |
			(divisor << PSPI_CTL1_SCDV6_0),
			&priv->pspi_regs->pspi_ctl1);

	/* Enabling the PSPI module, 8-bit,  */
	writew(readw(&priv->pspi_regs->pspi_ctl1) | (0x1 << PSPI_CTL1_SPIEN),
			&priv->pspi_regs->pspi_ctl1);

	return 0;
}

static int npcm_pspi_set_mode(struct udevice *bus, uint mode)
{
	struct npcm_pspi_priv *priv = dev_get_priv(bus);
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

static int npcm_spi_ofdata_to_platdata(struct udevice *bus)
{
	struct npcm_pspi_platdata *plat = dev_get_plat(bus);
	const void *blob = gd->fdt_blob;
	int node = dev_of_offset(bus);

	plat->regs = (phys_addr_t)dev_read_addr_ptr(bus);
	plat->max_hz = fdtdec_get_int(blob, node, "spi-max-frequency",
			500000);

	plat->dev_num = fdtdec_get_int(blob, node, "index", 0);

	return 0;
}

static int npcm_pspi_probe(struct udevice *bus)
{
	struct npcm_pspi_platdata *plat = dev_get_plat(bus);
	struct npcm_pspi_priv *priv = dev_get_priv(bus);
	int node = dev_of_offset(bus);
	int ret;

	ret = clk_get_by_index(bus, 0, &priv->pspi_clk);
	if (ret < 0) {
		printf("%s:Probe failed: Failed to get clk!\n", __func__);
		return ret;
	}

	priv->pspi_dev_num = (enum pspi_dev)plat->dev_num;
	priv->pspi_regs = (struct npcm_pspi_regs *)plat->regs;

	gpio_request_by_name_nodev(offset_to_ofnode(node), "cs-gpios", 0,
				&plat->cs_gpio, GPIOD_IS_OUT);

	return 0;
}


static const struct dm_spi_ops npcm_pspi_ops = {
	.claim_bus      = npcm_pspi_claim_bus,
	.release_bus    = npcm_pspi_release_bus,
	.xfer           = npcm_pspi_xfer,
	.set_speed      = npcm_pspi_set_speed,
	.set_mode       = npcm_pspi_set_mode,
	/*
	 * cs_info is not needed, since we require all chip selects to be
	 * in the device tree explicitly
	 */
};

static const struct udevice_id npcm_pspi_ids[] = {
	{ .compatible = "nuvoton,npcm845-pspi"},
	{ }
};

U_BOOT_DRIVER(npcm_pspi) = {
	.name   = "npcm_pspi",
	.id     = UCLASS_SPI,
	.of_match = npcm_pspi_ids,
	.ops    = &npcm_pspi_ops,
	.of_to_plat = npcm_spi_ofdata_to_platdata,
	.plat_auto = sizeof(struct npcm_pspi_platdata),
	.priv_auto = sizeof(struct npcm_pspi_priv),
	.probe  = npcm_pspi_probe,
};
