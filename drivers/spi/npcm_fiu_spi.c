// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#include <common.h>
#include <dm.h>
#include <spi.h>
#include <asm/arch/fiu.h>
#include <linux/iopoll.h>

struct npcm_fiu_priv {
	struct npcm_fiu_regs *regs;
};

static int npcm_fiu_spi_set_speed(struct udevice *bus, uint speed)
{
	return 0;
}

static int npcm_fiu_spi_set_mode(struct udevice *bus, uint mode)
{
	return 0;
}


static int fiu_uma_read(struct udevice *bus, u8 *buf, u32 data_size)
{
	struct npcm_fiu_priv *priv = dev_get_priv(bus);
	struct npcm_fiu_regs *regs = priv->regs;
	u32 data_reg[4];
	u32 val;
	int ret;

	/* Set data size */
	writel((data_size << FIU_UMA_CFG_RDATSIZ), &regs->uma_cfg);

	/* Initiate the read */
	writel(readl(&regs->uma_cts) | (1 << FIU_UMA_CTS_EXEC_DONE),
		&regs->uma_cts);

	/* wait for indication that transaction has terminated */
	ret = readl_poll_timeout(&regs->uma_cts, val,
			!(val & (1 << FIU_UMA_CTS_EXEC_DONE)), 1000000);
	if (ret) {
		printf("npcm_fiu: read timeout\n");
		return ret;
	}

	/* copy read data from FIU_UMA_DB0-3 regs to data buffer */
	if (data_size >= FIU_UMA_DATA_SIZE_1)
		data_reg[0] = readl(&regs->uma_dr0);
	if (data_size >= FIU_UMA_DATA_SIZE_5)
		data_reg[1] = readl(&regs->uma_dr1);
	if (data_size >= FIU_UMA_DATA_SIZE_9)
		data_reg[2] = readl(&regs->uma_dr2);
	if (data_size >= FIU_UMA_DATA_SIZE_13)
		data_reg[3] = readl(&regs->uma_dr3);

	memcpy(buf, data_reg, data_size);

	return 0;
}

static int fiu_uma_write(struct udevice *bus, const u8 *buf, u32 data_size)
{
	struct npcm_fiu_priv *priv = dev_get_priv(bus);
	struct npcm_fiu_regs *regs = priv->regs;
	u32 data_reg[4];
	u32 val;
	int ret;

	/* Set data size */
	writel((data_size << FIU_UMA_CFG_WDATSIZ), &regs->uma_cfg);

	/* Set the UMA data registers - FIU_UMA_DB0-3 */
	memcpy(data_reg, buf, data_size);

	if (data_size >= FIU_UMA_DATA_SIZE_1)
		writel(data_reg[0], &regs->uma_dw0);
	if (data_size >= FIU_UMA_DATA_SIZE_5)
		writel(data_reg[1], &regs->uma_dw1);
	if (data_size >= FIU_UMA_DATA_SIZE_9)
		writel(data_reg[2], &regs->uma_dw2);
	if (data_size >= FIU_UMA_DATA_SIZE_13)
		writel(data_reg[3], &regs->uma_dw3);

	/* Initiate the transaction */
	writel(readl(&regs->uma_cts) | (1 << FIU_UMA_CTS_EXEC_DONE),
		&regs->uma_cts);

	/* wait for indication that transaction has terminated */
	ret = readl_poll_timeout(&regs->uma_cts, val,
			!(val & (1 << FIU_UMA_CTS_EXEC_DONE)), 1000000);
	if (ret)
		printf("npcm_fiu: write timeout\n");

	return ret;
}

static int npcm_fiu_spi_xfer(struct udevice *dev, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct npcm_fiu_priv *priv = dev_get_priv(bus);
	struct npcm_fiu_regs *regs = priv->regs;
	struct dm_spi_slave_plat *slave_plat =
			dev_get_parent_plat(dev);
	const u8 *tx = dout;
	u8 *rx = din;
	int bytes = bitlen / 8;
	int ret = 0;
	int len;

	if (flags & SPI_XFER_BEGIN) {
		/* activate CS */
		writel((slave_plat->cs & 0x3) << FIU_UMA_CTS_DEV_NUM, &regs->uma_cts);
	}

	while (bytes) {
		len = (bytes > CHUNK_SIZE) ? CHUNK_SIZE : bytes;
		if (tx) {
			ret = fiu_uma_write(bus, tx, len);
			if (ret)
				break;
			tx += len;
		} else { /* rx */
			ret = fiu_uma_read(bus, rx, len);
			if (ret)
				break;
			rx += len;
		}
		bytes -= len;
	}

	if (flags & SPI_XFER_END) {
		/* deactivate CS  */
		writel((1 << FIU_UMA_CTS_SW_CS), &regs->uma_cts);
	}

	return ret;
}

static int npcm_fiu_spi_probe(struct udevice *bus)
{
	struct npcm_fiu_priv *priv = dev_get_priv(bus);

	debug("%s\n", __func__);
	priv->regs = (struct npcm_fiu_regs *)dev_read_addr_ptr(bus);

	return 0;
}

static const struct dm_spi_ops npcm_fiu_spi_ops = {
	.xfer           = npcm_fiu_spi_xfer,
	.set_speed      = npcm_fiu_spi_set_speed,
	.set_mode       = npcm_fiu_spi_set_mode,
};

static const struct udevice_id npcm_fiu_spi_ids[] = {
	{ .compatible = "nuvoton,npcm845-fiu" },
	{ }
};

U_BOOT_DRIVER(npcm_fiu_spi) = {
	.name   = "npcm_fiu_spi",
	.id     = UCLASS_SPI,
	.of_match = npcm_fiu_spi_ids,
	.ops    = &npcm_fiu_spi_ops,
	.priv_auto = sizeof(struct npcm_fiu_priv),
	.probe  = npcm_fiu_spi_probe,
};
