// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology.
 */

#include <common.h>
#include <dm.h>
#include <serial.h>
#include <asm/arch/uart.h>

struct npcm_serial_plat {
	struct npcm_uart *reg;
	u32 uart_clk;
};

static int npcm_serial_init(struct npcm_uart *uart)
{
	u8 val;

	/* Disable all UART interrupt */
	writeb(0, &uart->ier);

	/* Set port for 8 bit, 1 stop, no parity */
	val = LCR_WLS_8b;
	writeb(val, &uart->lcr);

	/* Set the RX FIFO trigger level, reset RX, TX FIFO */
	val = FCR_FME | FCR_RFR | FCR_TFR | FCR_RFITL_4B;
	writeb(val, &uart->fcr);

	return 0;
}

static int npcm_serial_pending(struct udevice *dev, bool input)
{
	struct npcm_serial_plat *plat = dev_get_plat(dev);
	struct npcm_uart *const uart = plat->reg;

	if (input)
		return (readb(&uart->lsr) & LSR_RFDR);
	else
		return !(readb(&uart->lsr) & LSR_THRE);

	return 0;
}

static int npcm_serial_putc(struct udevice *dev, const char ch)
{
	struct npcm_serial_plat *plat = dev_get_plat(dev);
	struct npcm_uart *const uart = plat->reg;

	while (!(readl(&uart->lsr) & LSR_THRE))
		;

	writeb(ch, &uart->thr);

	return 0;
}

static int npcm_serial_getc(struct udevice *dev)
{
	struct npcm_serial_plat *plat = dev_get_plat(dev);
	struct npcm_uart *const uart = plat->reg;

	while (!(readl(&uart->lsr) & LSR_RFDR))
		;

	return (int)(readb(&uart->rbr) & 0xff);
}

static int npcm_serial_setbrg(struct udevice *dev, int baudrate)
{
	struct npcm_serial_plat *plat = dev_get_plat(dev);
	struct npcm_uart *const uart = plat->reg;
	int ret = 0;
	u32 divisor;

	/* BaudOut = UART Clock  / (16 * [Divisor + 2]) */
	divisor = DIV_ROUND_CLOSEST(plat->uart_clk, 16 * baudrate + 2) - 2;

	writeb(readb(&uart->lcr) | LCR_DLAB, &uart->lcr);
	writeb(divisor & 0xff, &uart->dll);
	writeb(divisor >> 8, &uart->dlm);
	writeb(readb(&uart->lcr) & (~LCR_DLAB), &uart->lcr);

	return ret;
}

static int npcm_serial_probe(struct udevice *dev)
{
	struct npcm_serial_plat *plat = dev_get_plat(dev);
	struct npcm_uart *const uart = plat->reg;

	plat->reg = (struct npcm_uart *)dev_read_addr_ptr(dev);
	plat->uart_clk = dev_read_u32_default(dev, "clock-frequency", 0);
	npcm_serial_init(uart);

	return 0;
}

static const struct dm_serial_ops npcm_serial_ops = {
	.getc = npcm_serial_getc,
	.setbrg = npcm_serial_setbrg,
	.putc = npcm_serial_putc,
	.pending = npcm_serial_pending,
};

static const struct udevice_id npcm_serial_ids[] = {
	{ .compatible = "nuvoton,npcm750-uart" },
	{ .compatible = "nuvoton,npcm845-uart" },
	{ }
};

U_BOOT_DRIVER(serial_npcm) = {
	.name	= "serial_npcm",
	.id	= UCLASS_SERIAL,
	.of_match = npcm_serial_ids,
	.plat_auto  = sizeof(struct npcm_serial_plat),
	.probe = npcm_serial_probe,
	.ops	= &npcm_serial_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
