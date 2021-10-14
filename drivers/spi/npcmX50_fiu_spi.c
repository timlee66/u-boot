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
#include <asm/arch/spi_flash.h>
#include <spi_flash.h>

//#undef _DEBUG
//#define _DEBUG 1

DECLARE_GLOBAL_DATA_PTR;

enum npcmX50_sf_state {
	SF_CMD		    = 0,
	SF_ID		    = 1,
	SF_ADDR		    = 2,
	SF_READ		    = 3,
	SF_WRITE	    = 4,
	SF_ERASE	    = 5,
	SF_READ_STATUS	    = 6,
	SF_READ_STATUS1	    = 7,
	SF_WRITE_STATUS	    = 8,
	SF_READ_BAR,
	SF_WRITE_BAR,
};

/* Bits for the status register */
#define STAT_WIP        (1 << 0)
#define STAT_WEL        (1 << 1)

static const char *npcmX50_sf_state_name(enum npcmX50_sf_state state)
{
	static const char * const states[] = {
		"CMD", "ID", "ADDR", "READ", "WRITE", "ERASE", "READ_STATUS",
		"READ_STATUS1", "WRITE_STATUS", "READ_BAR", "WRITE_BAR"
	};
	return states[state];
}

struct npcmX50_fiu_spi_platdata {
	void* regs;
	s32 frequency;
	u32 dev_num;
};

struct npcmX50_spi_op {
	u32 addr;
	u32 rd_size;
	u32 wr_size;
	u8 opcode;
	u8 cmd_len;
	u8 addr_len;
	u8 dummy_len;
};

struct npcmX50_fiu_spi_priv {
	struct npcmX50_fiu_regs *regs;
	u32 cs;
	enum fiu_moudle_tag dev_num;
	enum npcmX50_sf_state state;
	u16 status; /* The current flash status (see STAT_XXX defines above) */
	u32 cmd;
};

static void fiu_spi_cs_activate(struct udevice *dev)
{
	struct udevice *bus = dev->parent;
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);

	priv->state = SF_CMD;
	priv->cmd = SF_CMD;
}

static void fiu_spi_cs_deactivate(struct udevice *dev)
{
	return;
}

static int npcmX50_fiu_spi_set_speed(struct udevice *bus, uint speed)
{
	return 0;
}

static int npcmX50_fiu_spi_set_mode(struct udevice *bus, uint mode)
{
	return 0;
}

static int npcmX50_fiu_spi_claim_bus(struct udevice *dev)
{
	return 0;
}

static int npcmX50_fiu_spi_release_bus(struct udevice *dev)
{
	return 0;
}

static int fiu_uma_read(struct udevice *bus, u8 *buf, u32 data_size,
		struct npcmX50_spi_op *op)
{
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);
	struct npcmX50_fiu_regs *regs = priv->regs;
	u32 data_reg[4];
	u32 uma_cfg;

	uma_cfg = (1 << FIU_UMA_CFG_CMDSIZ) |
				(op->addr_len << FIU_UMA_CFG_ADDSIZ) |
				(op->dummy_len << FIU_UMA_CFG_DBSIZ) |
				(data_size << FIU_UMA_CFG_RDATSIZ);
	/* Set UMA CFG */
	writel(uma_cfg, &regs->uma_cfg);

	/* Set the UMA address registers */
	writel(op->addr, &regs->uma_addr);

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

	memcpy(buf, data_reg, data_size);

	return 0;
}

static int fiu_uma_write(struct udevice *bus, const u8 *buf, u32 data_size,
		struct npcmX50_spi_op *op)
{
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);
	struct npcmX50_fiu_regs *regs = priv->regs;
	u32 data_reg[4];
	u32 uma_cfg;

	uma_cfg = (1 << FIU_UMA_CFG_CMDSIZ) |
				(op->addr_len << FIU_UMA_CFG_ADDSIZ) |
				(op->dummy_len << FIU_UMA_CFG_DBSIZ) |
				(data_size << FIU_UMA_CFG_WDATSIZ);
	/* Set UMA CFG */
	writel(uma_cfg, &regs->uma_cfg);

	/* Set the UMA address registers */
	writel(op->addr, &regs->uma_addr);

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

	/* Initiate the read */
	writel(readl(&regs->uma_cts) | (1 << FIU_UMA_CTS_EXEC_DONE),
		&regs->uma_cts);

	/* wait for indication that transaction has terminated */
	while ((readl(&regs->uma_cts) & (1 << FIU_UMA_CTS_EXEC_DONE))
		== FIU_TRANS_STATUS_IN_PROG);

	return 0;
}

static int npcmX50_sf_process_cmd(struct udevice *dev, const u8 *rx, u8 *tx)
{
	struct udevice *bus = dev->parent;
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);
	struct npcmX50_fiu_regs *regs = priv->regs;
	enum npcmX50_sf_state oldstate = priv->state;
	struct dm_spi_slave_plat *slave_plat =
			dev_get_parent_plat(dev);
	struct npcmX50_spi_op spi_op;

	priv->cmd = rx[0];
	/* set device number - DEV_NUM in FIU_UMA_CTS.
	   Select the chip select to be used in the following
	   UMA transaction */
	writel((readl(&regs->uma_cts) & ~(0x03 << FIU_UMA_CTS_DEV_NUM))
			| ((slave_plat->cs & 0x3) << FIU_UMA_CTS_DEV_NUM),
			&regs->uma_cts);

	/* set transaction code in FIU_UMA_CODE */
	writel((readl(&regs->uma_cmd) & ~(0xff << FIU_UMA_CMD_CMD))
			| (priv->cmd << FIU_UMA_CMD_CMD), &regs->uma_cmd);

	memset((u8 *)&spi_op, 0, sizeof(struct npcmX50_spi_op));
	spi_op.opcode = priv->cmd;

	switch (priv->cmd) {
		case SPI_READ_JEDEC_ID_CMD:
			priv->state = SF_ID;
			priv->cmd = SF_ID;
			break;
		case SPI_READ_STATUS_REG_CMD:
			priv->state = SF_READ_STATUS;
			break;
		case SPI_WRITE_STATUS_REG_CMD:
			priv->state = SF_WRITE_STATUS;
			break;
		case SPI_EXTNADDR_RDEAR_CMD:
			priv->state = SF_READ_BAR;
			break;
		case SPI_EXTNADDR_WREAR_CMD:
			priv->state = SF_WRITE_BAR;
			break;
		case SPI_WRITE_ENABLE_CMD:
			fiu_uma_write(bus, NULL, 0, &spi_op);
			priv->status |= STAT_WEL;
			break;
		case SPI_WRITE_DISABLE_CMD:
			priv->status &= ~STAT_WEL;
			break;
		case SPI_4K_SECTOR_ERASE_CMD:
		case SPI_64K_BLOCK_ERASE_CMD:
			priv->state = SF_ADDR;
			break;
		case SPI_PAGE_PRGM_CMD:
			writel(readl(&regs->uma_cts)
				& ~(1 << FIU_UMA_CTS_SW_CS), &regs->uma_cts);
			priv->state = SF_ADDR;
			break;
		case SPINOR_OP_READ_FAST:
			priv->state = SF_ADDR;
			break;
		default:
			printf("%s: unknown cmd:0x%x\n", __func__, priv->cmd);
			return -EIO;
	}

	if (oldstate != priv->state)
		debug(" %s: transition to %s state\n", dev->name,
				npcmX50_sf_state_name(priv->state));

	return 0;
}

static int npcmX50_fiu_spi_xfer(struct udevice *dev, unsigned int bitlen,
			const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct npcmX50_fiu_spi_priv *priv = dev_get_priv(bus);
	struct npcmX50_fiu_regs *regs = priv->regs;
	struct dm_spi_slave_plat *slave_plat =
			dev_get_parent_plat(dev);
	struct npcmX50_spi_op spi_op;
	const u8 *rx = dout;
	uint8_t *tx = din;
	int bytes = bitlen / 8;
	u32 address, pos = 0;
	u32 remain_data = 0;
	int ret;
	int read_bytes;

	if (flags & SPI_XFER_BEGIN)
		fiu_spi_cs_activate(dev);

	if (priv->state == SF_CMD) {
		ret = npcmX50_sf_process_cmd(dev, rx, tx);
		if (ret)
			return ret;
		++pos;
		memset((u8 *)&spi_op, 0, sizeof(struct npcmX50_spi_op));
		spi_op.opcode = priv->cmd;
	}

	if (priv->state == SF_WRITE)
		remain_data = bytes % CHUNK_SIZE;


	while (pos < bytes) {
		switch (priv->state) {
		case SF_ID:
			fiu_uma_read(bus, tx, bytes, &spi_op);
			pos += bytes;
			break;
		case SF_READ_STATUS:
			fiu_uma_read(bus, tx, 1, &spi_op);
			pos ++;
			break;
		case SF_WRITE_STATUS:
			fiu_uma_write(bus, rx, 1, &spi_op);
			pos ++;
			break;
		case SF_READ_BAR:
			fiu_uma_read(bus, tx, 1, &spi_op);
			pos ++;
			break;
		case SF_WRITE_BAR:
			fiu_uma_write(bus, rx, 1, &spi_op);
			pos ++;
			break;
		case SF_ADDR:
			address = (rx[1] << 16) | (rx[2] << 8) | (rx[3] << 0);
			pos += 3;
			switch (priv->cmd) {
			case SPI_4K_SECTOR_ERASE_CMD:
			case SPI_64K_BLOCK_ERASE_CMD:
				if (!(priv->status & STAT_WEL)) {
					printf("%s: write enable not set"
						" before erase!\n",
						__func__);
				}
				spi_op.addr = address;
				spi_op.addr_len = 3;
				spi_op.dummy_len = 0;
				fiu_uma_write(bus, NULL, 0, &spi_op);
				priv->status &= ~STAT_WEL;
				break;
			case SPI_PAGE_PRGM_CMD:
				spi_op.addr = address;
				spi_op.addr_len = 3;
				spi_op.dummy_len = 0;
				priv->state = SF_WRITE;
				fiu_uma_write(bus, NULL, 0, &spi_op);
				break;
			case SPINOR_OP_READ_FAST:
				spi_op.addr = address;
				spi_op.addr_len = 3;
				spi_op.dummy_len = 1;
				priv->state = SF_READ;
				pos++;
				break;
			}
			break;
		case SF_READ:
			if (bytes - pos > 16)
				read_bytes = 16;
			else
				read_bytes = bytes - pos;
			fiu_uma_read(bus, tx, read_bytes, &spi_op);
			pos += read_bytes;
			tx += read_bytes;
			spi_op.addr += read_bytes;
			break;
		case SF_WRITE:
			if (!(priv->status & STAT_WEL)) {
				printf("%s: write enable not set"
					" before write!\n",
					__func__);
				goto done;
			}
			spi_op.addr_len = 0;
			spi_op.dummy_len = 0;

			writel((readl(&regs->uma_cts) &
				~(0x03 << FIU_UMA_CTS_DEV_NUM))
				| ((slave_plat->cs & 0x3) << FIU_UMA_CTS_DEV_NUM),
				&regs->uma_cts);

			/* first data byte is sent in command phase */
			writel((readl(&regs->uma_cmd) &
				~(0xff << FIU_UMA_CMD_CMD))
				| (rx[0] << FIU_UMA_CMD_CMD),
				&regs->uma_cmd);

			if ((pos + remain_data) < bytes) {
				fiu_uma_write(bus, &rx[1], CHUNK_SIZE - 1, &spi_op);
				pos = pos + CHUNK_SIZE;
				rx = rx + CHUNK_SIZE;
			} else if (remain_data) {
				fiu_uma_write(bus, &rx[1], remain_data - 1, &spi_op);
				pos = pos + remain_data;
			}

			if (pos >= bytes) {
				writel(readl(&regs->uma_cts) |
					(1 << FIU_UMA_CTS_SW_CS),
					&regs->uma_cts);
				priv->status &= ~STAT_WEL;
			}
			break;
		default:
			printf("%s: no idea what to do.\n", __func__);
			goto done;
		}
	}

done:
	if (flags & SPI_XFER_END)
		fiu_spi_cs_deactivate(dev);

	return pos == bytes ? 0 : -EIO;
}

static int npcmX50_fiu_spi_ofdata_to_platdata(struct udevice *bus)
{
	struct npcmX50_fiu_spi_platdata *plat = dev_get_plat(bus);
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
	struct npcmX50_fiu_spi_platdata *plat = dev_get_plat(bus);
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
	.of_to_plat = npcmX50_fiu_spi_ofdata_to_platdata,
	.plat_auto = sizeof(struct npcmX50_fiu_spi_platdata),
	.priv_auto = sizeof(struct npcmX50_fiu_spi_priv),
	.probe  = npcmX50_fiu_spi_probe,
};
