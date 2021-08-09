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
#include <fdtdec.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gcr.h>
#include <asm/arch/fiu.h>

DECLARE_GLOBAL_DATA_PTR;

struct npcmX50_fiu_spi_platdata {
	void* regs;
	s32 frequency;
	u32 dev_num;
};

struct npcmX50_fiu_spi_priv {
	struct npcmX50_fiu_regs *regs;
	u32 cs;
	enum fiu_moudle_tag dev_num;
};

static void gcr_muxfiu(enum fiu_moudle_tag dev_num,
		bool cs0_en, bool cs1_en, bool cs2_en,
		bool cs3_en, bool quad_mode)
{
#if defined (CONFIG_TARGET_ARBEL)
	struct npcm850_gcr *gcr = (struct npcm850_gcr *)npcm850_get_base_gcr();
#elif defined (CONFIG_TARGET_POLEG)
	struct npcm750_gcr *gcr = (struct npcm750_gcr *)npcm750_get_base_gcr();
#endif

	switch (dev_num) {
	case FIU_MODULE_0:
		/* config CS */
		if (cs0_en)
			/* nothing to do */;
		/* config CS1 */
		if (cs1_en)
			writel(readl(&gcr->mfsel1) | (1 << MFSEL1_S0CS1SEL),
				&gcr->mfsel1);
		/* config CS2 */
#if defined (CONFIG_TARGET_POLEG)
		if (cs2_en)
			writel(readl(&gcr->mfsel1) | (1 << MFSEL1_S0CS2SEL),
				&gcr->mfsel1);
		/* config CS3 */
		if (cs3_en)
			writel(readl(&gcr->mfsel1) | (1 << MFSEL1_S0CS3SEL),
				&gcr->mfsel1);
#endif
		/* select io bus width (1/2/4  <=> single/dual/quad ) */
		if (quad_mode) {
#if defined (CONFIG_TARGET_POLEG)
			/*  0: GPIO33/SPI0D2 ,1: nSPI0CS2 */
			writel(readl(&gcr->mfsel1) & ~(1 << MFSEL1_S0CS2SEL),
				&gcr->mfsel1);
			writel(readl(&gcr->mfsel1) & ~(1 << MFSEL1_S0CS3SEL),
				&gcr->mfsel1);
#endif
		}
		break;
#if defined (CONFIG_TARGET_ARBEL)
	case FIU_MODULE_1:
		/* Select SPI1 */
		writel(readl(&gcr->mfsel3) & ~(1 << MFSEL3_FIN1916SELB),
			&gcr->mfsel3);
		writel(readl(&gcr->mfsel6) & ~(1 << MFSEL6_FM1SEL),
			&gcr->mfsel6);
		writel(readl(&gcr->mfsel3) | (1 << MFSEL3_SPI1SEL),
			&gcr->mfsel3);
			
		/* config CS */
		if (cs0_en)
			/* nothing to do */;
		/* config CS1 */
		if (cs1_en)
			writel(readl(&gcr->mfsel5) | (1 << MFSEL5_NSPI1CS1SEL),
				&gcr->mfsel5);
		/* Config CS2 */
		if (cs2_en)
		{
			writel(readl(&gcr->mfsel5) & ~(1 << MFSEL5_SPI1D23SEL),
				&gcr->mfsel5);
			writel(readl(&gcr->mfsel7) & ~(1 << MFSEL7_SMB15SELB),
				&gcr->mfsel7);
			writel(readl(&gcr->mfsel5) | (1 << MFSEL5_NSPI1CS2SEL),
				&gcr->mfsel5);
		}
		/* Config CS3 */
		if (cs3_en)
		{
			writel(readl(&gcr->mfsel5) & ~(1 << MFSEL5_SPI1D23SEL),
				&gcr->mfsel5);
			writel(readl(&gcr->mfsel7) & ~(1 << MFSEL7_SMB15SELB),
				&gcr->mfsel7);
			writel(readl(&gcr->mfsel5) | (1 << MFSEL5_NSPI1CS3SEL),
				&gcr->mfsel5);
		}
		/* select io bus width (1/2/4  <=> single/dual/quad ) */
		if (quad_mode) {
			writel(readl(&gcr->mfsel5) & ~(1 << MFSEL5_NSPI1CS2SEL),
				&gcr->mfsel5);
			writel(readl(&gcr->mfsel5) & ~(1 << MFSEL5_NSPI1CS3SEL),
				&gcr->mfsel5);
			writel(readl(&gcr->mfsel7) & ~(1 << MFSEL7_SMB15SELB),
				&gcr->mfsel7);
			writel(readl(&gcr->mfsel5) | (1 << MFSEL5_SPI1D23SEL),
				&gcr->mfsel5);
		}
		break;
#endif
	case FIU_MODULE_3:
		/* Select SPI3 */
#if defined (CONFIG_TARGET_ARBEL)
		writel(readl(&gcr->mfsel6) & ~(1 << MFSEL6_GPIO1836SEL),
			&gcr->mfsel6);
#endif
		writel(readl(&gcr->mfsel4) | (1 << MFSEL4_SP3SEL),
			&gcr->mfsel4);
		/* config CS */
		if (cs0_en)
			/* nothing to do */;
		/* config CS1 */
		if (cs1_en)
			writel(readl(&gcr->mfsel4) | (1 << MFSEL4_S3CS1SEL),
				&gcr->mfsel4);
		/* Config CS2 */
		if (cs2_en)
		{
#if defined (CONFIG_TARGET_ARBEL)
			writel(readl(&gcr->mfsel7) & ~(1 << MFSEL7_GPIO1889SEL),
				&gcr->mfsel7);
			writel(readl(&gcr->mfsel4) & ~(1 << MFSEL4_SP3QSEL),    
				&gcr->mfsel4);
#endif
			writel(readl(&gcr->mfsel4) | (1 << MFSEL4_S3CS2SEL),
				&gcr->mfsel4);
		}
		/* Config CS3 */
		if (cs3_en)
		{
#if defined (CONFIG_TARGET_ARBEL)
			writel(readl(&gcr->mfsel7) & ~(1 << MFSEL7_GPIO1889SEL),
				&gcr->mfsel7);
			writel(readl(&gcr->mfsel4) & ~(1 << MFSEL4_SP3QSEL),    
				&gcr->mfsel4);
#endif
			writel(readl(&gcr->mfsel4) | (1 << MFSEL4_S3CS3SEL),
				&gcr->mfsel4);
		}
		/* select io bus width (1/2/4  <=> single/dual/quad ) */
		if (quad_mode) {
			writel(readl(&gcr->mfsel4) & ~(1 << MFSEL4_S3CS2SEL),
				&gcr->mfsel4);
			writel(readl(&gcr->mfsel4) & ~(1 << MFSEL4_S3CS3SEL),
				&gcr->mfsel4);
#if defined (CONFIG_TARGET_ARBEL)
			writel(readl(&gcr->mfsel7) & ~(1 << MFSEL7_GPIO1889SEL),
				&gcr->mfsel7);
#endif
			writel(readl(&gcr->mfsel4) | (1 << MFSEL4_SP3QSEL),
				&gcr->mfsel4);
		}
		break;
	case FIU_MODULE_X:
		/* config CS */
		if (cs0_en)
			/* nothing to do */;
		/* config CS1 */
		if (cs1_en)
			writel(readl(&gcr->mfsel4) | (1 << MFSEL4_SXCS1SEL),
				&gcr->mfsel4);
		writel(readl(&gcr->mfsel4) | (1 << MFSEL4_SPXSEL),
			&gcr->mfsel4);
		break;
	default:
		break;
	};
}

static int npcmX50_fiu_spi_set_speed(struct udevice *bus, uint speed)
{
	return 0;
}

static int npcmX50_fiu_spi_set_mode(struct udevice *bus, uint mode)
{
	return 0;
}

static int npcmX50_fiu_spi_release_bus(struct udevice *dev)
{
	return 0;
}

static int fiu_uma_read(struct udevice *bus, u32 address, uint8_t *rx,
			bool is_address_size, u32 data_size)
{
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);
	struct npcmX50_fiu_regs *regs = priv->regs;
	u32 data_reg[4];
	u32 uma_cfg = 0;
	u32 address_size = 0;

	//uma_cfg = uma_cfg | (1 << FIU_UMA_CFG_CMDSIZ);

	if (is_address_size) {
		address_size = 3;

		uma_cfg = (uma_cfg & ~(0x07 << FIU_UMA_CFG_ADDSIZ))
				| (address_size << FIU_UMA_CFG_ADDSIZ);

		/* Set the UMA address registers */
		writel(address, &regs->uma_addr);
	}

	/* Set data size and direction */
	uma_cfg = (uma_cfg & ~(0x1f << FIU_UMA_CFG_RDATSIZ)) |
		(data_size << FIU_UMA_CFG_RDATSIZ);
	uma_cfg = uma_cfg & ~(0x1f << FIU_UMA_CFG_WDATSIZ);

	/* Set UMA CFG */
	writel(uma_cfg, &regs->uma_cfg);

	/* Initiate the read */
	writel(readl(&regs->uma_cts) | (1 << FIU_UMA_CTS_EXEC_DONE),
		&regs->uma_cts);

	/* wait for indication that transaction has terminated */
	while ((readl(&regs->uma_cts) & (1 << FIU_UMA_CTS_EXEC_DONE))
		== FIU_TRANS_STATUS_IN_PROG);

	/* copy read data from FIU_UMA_DB0-3 regs to data buffer */
	/* Set the UMA data registers - FIU_UMA_DB0-3 */
	if (data_size >= FIU_UMA_DATA_SIZE_1)
		data_reg[0] = readl(&regs->uma_dr0);
	if (data_size >= FIU_UMA_DATA_SIZE_5)
		data_reg[1] = readl(&regs->uma_dr1);
	if (data_size >= FIU_UMA_DATA_SIZE_9)
		data_reg[2] = readl(&regs->uma_dr2);
	if (data_size >= FIU_UMA_DATA_SIZE_13)
		data_reg[3] = readl(&regs->uma_dr3);

	memcpy(rx, data_reg, data_size);

	return 0;
}

static int fiu_uma_write(struct udevice *bus, u32 address, const uint8_t *tx,
			bool is_address_size, u32 data_size)
{
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);
	struct npcmX50_fiu_regs *regs = priv->regs;
	u32 data_reg[4];
	u32 uma_cfg = 0;
	u32 address_size = 0;

	//uma_cfg = uma_cfg | (1 << FIU_UMA_CFG_CMDSIZ);

	if (is_address_size) {
		address_size = 3;

		uma_cfg = (uma_cfg & ~(0x07 << FIU_UMA_CFG_ADDSIZ))
				| (address_size << FIU_UMA_CFG_ADDSIZ);
		/* Set the UMA address registers */
		writel(address, &regs->uma_addr);
	}

	/* Set the UMA data registers - FIU_UMA_DB0-3 */
	if (data_size) {
		memcpy(data_reg, tx, data_size);

		if (data_size >= FIU_UMA_DATA_SIZE_1)
			writel(data_reg[0], &regs->uma_dw0);
		if (data_size >= FIU_UMA_DATA_SIZE_5)
			writel(data_reg[1], &regs->uma_dw1);
		if (data_size >= FIU_UMA_DATA_SIZE_9)
			writel(data_reg[2], &regs->uma_dw2);
		if (data_size >= FIU_UMA_DATA_SIZE_13)
			writel(data_reg[3], &regs->uma_dw3);
	}

	/* Set data size and direction */
	uma_cfg = (uma_cfg & ~(0x1f << FIU_UMA_CFG_WDATSIZ)) |
		(data_size << FIU_UMA_CFG_WDATSIZ);
	uma_cfg = uma_cfg & ~(0x1f << FIU_UMA_CFG_RDATSIZ);

	/* Set UMA status */
	writel(uma_cfg, &regs->uma_cfg);

	/* Initiate the write */
	writel(readl(&regs->uma_cts) | (1 << FIU_UMA_CTS_EXEC_DONE),
		&regs->uma_cts);

	/* wait for indication that transaction has terminated */
	while ((readl(&regs->uma_cts) & (1 << FIU_UMA_CTS_EXEC_DONE))
		== FIU_TRANS_STATUS_IN_PROG);

	return 0;
}

static int npcmX50_fiu_spi_xfer(struct udevice *dev, unsigned int bitlen,
			const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);
	struct npcmX50_fiu_regs *regs = priv->regs;
	struct dm_spi_slave_platdata *slave_plat =
			dev_get_parent_platdata(dev);
	const uint8_t *tx = dout;
	uint8_t *rx = din;
	int bytes = bitlen / 8;
	u32 pos = 0;

	if (flags & SPI_XFER_BEGIN)
		//if (tx && tx[0] == CMD_EXTNADDR_RDEAR){
		//}
		/* set device number - DEV_NUM in FIU_UMA_CTS.
		   Select the chip select to be used in the following
		   UMA transaction and set CS low */
		writel((readl(&regs->uma_cts) & ~(0x03 << FIU_UMA_CTS_DEV_NUM) & ~(1 << FIU_UMA_CTS_SW_CS))
			| ((slave_plat->cs & 0x3) << FIU_UMA_CTS_DEV_NUM),
			&regs->uma_cts);

	if ((flags & SPI_XFER_MMAP) || (flags & SPI_XFER_MMAP_END))
		goto done;

	while (pos < bytes) {
		if (tx) {
			if ((pos + CHUNK_SIZE) <= bytes) {
				fiu_uma_write(bus, 0x0, tx, false,
						CHUNK_SIZE);
				pos = pos + CHUNK_SIZE;
				tx = tx + CHUNK_SIZE;
			} else if (bytes - pos) {
				fiu_uma_write(bus, 0x0, tx, false,
						bytes - pos);
				pos = bytes;
			}
		}
		else { // rx
			if ((pos + CHUNK_SIZE) <= bytes) {
				fiu_uma_read(bus, 0x0, rx, false,
						CHUNK_SIZE);
				pos = pos + CHUNK_SIZE;
				rx = rx + CHUNK_SIZE;
			} else if (bytes - pos) {
				fiu_uma_read(bus, 0x0, rx, false,
						bytes - pos);
				pos = bytes;
			}
		}
	}

done:
	if (flags & SPI_XFER_END)			
		/* set CS high  */
		writel((readl(&regs->uma_cts) | (1 << FIU_UMA_CTS_SW_CS)), &regs->uma_cts);

	return pos == bytes ? 0 : -EIO;
}

static int npcmX50_fiu_spi_claim_bus(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);
	struct npcmX50_fiu_regs *regs = priv->regs;
	struct dm_spi_slave_platdata *slave_plat =
			dev_get_parent_platdata(dev);
	bool cs0_en, cs1_en, cs2_en, cs3_en, quad_mode;

	cs0_en = cs1_en = cs2_en = cs3_en = quad_mode = false;

	switch (slave_plat->cs) {
	case 0:
		cs0_en = true;
		quad_mode = true;
		break;
	case 1:
		cs1_en = true;
		quad_mode = true;
		break;
	case 2:
		cs2_en = true;
		break;
	case 3:
		cs3_en = true;
		break;
	default:
		cs0_en = true;
		quad_mode = true;	
		break;
	};

	if (priv->dev_num == FIU_MODULE_X)
	{
		writel(0x0300100B, &regs->drd_cfg);     /* FIU-X Change default drd_cfg register value to be regular FIU using a regular flash */
		writel(0x0000000B, &regs->fiu_cfg);     /* FIU-X Change default fiu_cfg register value to be regular FIU using a regular flash */		
	}
	
	/* SPI flash init */
	gcr_muxfiu(priv->dev_num, cs0_en, cs1_en, cs2_en, cs3_en, quad_mode);
	return 0;
}

static int npcmX50_fiu_spi_ofdata_to_platdata(struct udevice *bus)
{
	struct npcmX50_fiu_spi_platdata *plat = dev_get_platdata(bus);
	const void *blob = gd->fdt_blob;
	int node = dev_of_offset(bus);

	plat->regs = dev_read_addr_ptr(bus);

	/* Use 500KHz as a suitable default */
	plat->frequency = fdtdec_get_int(blob, node, "spi-max-frequency",
			500000);

	plat->dev_num = fdtdec_get_int(blob, node, "index", 0);

	return 0;
}

static int npcmX50_fiu_spi_probe(struct udevice *bus)
{
	struct npcmX50_fiu_spi_platdata *plat = dev_get_platdata(bus);
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);

	debug("%s\n", __func__);
	priv->regs = (struct npcmX50_fiu_regs *)plat->regs;
	priv->dev_num = (enum fiu_moudle_tag)plat->dev_num;

	return 0;
}

static const struct dm_spi_ops npcmX50_fiu_spi_ops = {
	.claim_bus      = npcmX50_fiu_spi_claim_bus,
	.release_bus    = npcmX50_fiu_spi_release_bus,
	.xfer           = npcmX50_fiu_spi_xfer,
	.set_speed      = npcmX50_fiu_spi_set_speed,
	.set_mode       = npcmX50_fiu_spi_set_mode,
	/*
	 * cs_info is not needed, since we require all chip selects to be
	 * in the device tree explicitly
	 */
};

static const struct udevice_id npcmX50_fiu_spi_ids[] = {
	{ .compatible = "nuvoton,npcmX50-fiu" },
	{ }
};

U_BOOT_DRIVER(npcmX50_fiu_spi) = {
	.name   = "npcmX50_fiu_spi",
	.id     = UCLASS_SPI,
	.of_match = npcmX50_fiu_spi_ids,
	.ops    = &npcmX50_fiu_spi_ops,
	.ofdata_to_platdata = npcmX50_fiu_spi_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct npcmX50_fiu_spi_platdata),
	.priv_auto_alloc_size = sizeof(struct npcmX50_fiu_spi_priv),
	.probe  = npcmX50_fiu_spi_probe,
};
