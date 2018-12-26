/*
	* Copyright 2011, Marvell Semiconductor Inc.
	* Lei Wen <leiwen@marvell.com>
	*
	* SPDX-License-Identifier:	GPL-2.0+
	*
	* Back ported to the 8xx platform (from the 8260 platform) by
	* Murray.Jensen@cmst.csiro.au, 27-Jan-01.
	*/

#include <common.h>
#include <asm/io.h>
#include <malloc.h>
#include <mmc.h>
#include <sdhci.h>


/*---------------------------------------------------------------------------------------------------------*/
/* SD Host Control Register(SDHC_BA)                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define NPCMX50_PA_SDHC(module)         SD_BASE_ADDR(module)

#ifndef _PALLADIUM_
#define SDHC_INTERRUPT
#endif

#define CONFIG_MMC_SDMA          /* Remove to enable fatwrite from flash address to MMC as file */

/*---------------------------------------------------------------------------------------------------------*/
/* For debug prints                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*#define DEBUG_SDHC */
#ifdef DEBUG_SDHC
	#define SD_MSG(f, x...)     printf("NPCMX50 SD/MMC [%s()]: " f, __func__,## x)
#else
	#define SD_MSG(f, x...)
#endif

#define DEBUG_CMD     /* Comment when not needed */
#ifdef DEBUG_CMD
	#define DBG_MSG(f, x...)     printf(f, ## x)
#else
	#define DBG_MSG(f, x...)
#endif


#define NUM_SLOTS   2

UINT32 CLK_ConfigureSDClock (UINT32 sdNum);
HAL_STATUS CLK_ResetSD(UINT32 sdNum);

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_FIXED_SDHCI_ALIGNED_BUFFER)
void *aligned_buffer = (void *)CONFIG_FIXED_SDHCI_ALIGNED_BUFFER;
#else
void *aligned_buffer;
#endif

struct sdhci_host *global_host[NUM_SLOTS];


inline void set_bit(int nr, volatile void *addr)
{
	int	mask;
	unsigned int *a = (unsigned int *) addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	*a |= mask;
}

inline void clear_bit(int nr, volatile void *addr)
{
	int	mask;
	unsigned int *a = (unsigned int *) addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	*a &= ~mask;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sdhci_dumpregs                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  host -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine dumps registers values                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static void sdhci_dumpregs(struct sdhci_host* host)
{
	int i=0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* * - Read registers and print them out.                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	printf("NPCMX50 SD/MMC: ============== REGISTER DUMP ==============\n");

	printf(": Sys addr: 0x%08x | Version:  0x%08x\n",
        sdhci_readl(host, SDHCI_DMA_ADDRESS),
        sdhci_readw(host, SDHCI_HOST_VERSION));
	printf(": Blk size: 0x%08x | Blk cnt:  0x%08x\n",
        sdhci_readw(host, SDHCI_BLOCK_SIZE),
        sdhci_readw(host, SDHCI_BLOCK_COUNT));
	printf(": Command:  0x%08x | SW Reset: 0x%08x\n",
        sdhci_readw(host, SDHCI_COMMAND),
        sdhci_readb(host, SDHCI_SOFTWARE_RESET));
	printf(": Argument: 0x%08x | Trn mode: 0x%08x\n",
        sdhci_readl(host, SDHCI_ARGUMENT),
        sdhci_readw(host, SDHCI_TRANSFER_MODE));
	printf(": Present:  0x%08x | Host ctl: 0x%08x\n",
        sdhci_readl(host, SDHCI_PRESENT_STATE),
        sdhci_readb(host, SDHCI_HOST_CONTROL));
	printf(": Power:    0x%08x | Blk gap:  0x%08x\n",
        sdhci_readb(host, SDHCI_POWER_CONTROL),
        sdhci_readb(host, SDHCI_BLOCK_GAP_CONTROL));
	printf(": Wake-up:  0x%08x | Clock:    0x%08x\n",
        sdhci_readb(host, SDHCI_WAKE_UP_CONTROL),
        sdhci_readw(host, SDHCI_CLOCK_CONTROL));
	printf(": Timeout:  0x%08x | Int stat: 0x%08x\n",
        sdhci_readb(host, SDHCI_TIMEOUT_CONTROL),
        sdhci_readl(host, SDHCI_INT_STATUS));
	printf(": Int enab: 0x%08x | Sig enab: 0x%08x\n",
        sdhci_readl(host, SDHCI_INT_ENABLE),
        sdhci_readl(host, SDHCI_SIGNAL_ENABLE));
	printf(": AC12 err: 0x%08x | Slot int: 0x%08x\n",
        sdhci_readw(host, SDHCI_ACMD12_ERR),
        sdhci_readw(host, SDHCI_SLOT_INT_STATUS));
	printf(": Caps:     0x%08x | Max curr: 0x%08x\n",
        sdhci_readl(host, SDHCI_CAPABILITIES),
        sdhci_readl(host, SDHCI_MAX_CURRENT));

	printf("Response = ");
	for (i=0; i<16; i++)
        printf("%x ",  sdhci_readb(host, SDHCI_RESPONSE + i));
	printf("\n");


	printf("NPCMX50 SD/MMC: ===========================================\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sdhci_activate_led                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  host -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine activates LED                                                             */
/*---------------------------------------------------------------------------------------------------------*/
static void sdhci_activate_led(struct sdhci_host *host)
{
	u8 ctrl;

	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	ctrl |= SDHCI_CTRL_LED;
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sdhci_deactivate_led                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  host -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine deactivates LED                                                           */
/*---------------------------------------------------------------------------------------------------------*/
static void sdhci_deactivate_led(struct sdhci_host *host)
{
	u8 ctrl;

	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_LED;
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
}

#if defined (NPCM750)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sdhci1_irq                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles controller IRQs                                                   */
/*---------------------------------------------------------------------------------------------------------*/
static HAL_STATUS sdhci1_irq(UINT32 devNum)
{
	HAL_STATUS result = HAL_OK;;
	UINT32 intmask;
	struct sdhci_host *host = global_host[devNum];

	host->ioaddr = (void *)NPCMX50_PA_SDHC(devNum);
	intmask = sdhci_readl(host, SDHCI_INT_STATUS);

	/*-----------------------------------------------------------------------------------------------------*/
	/* If no interrupt or invalid status, ignore it.                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	if (!intmask || intmask == 0xffffffff)
	{
        SD_MSG("Got invalid interrupt1: 0x%08x\n", intmask);
        return result;
	}

	SD_MSG("Got interrupt1: intmask = 0x%08x\n", intmask);

	/*-----------------------------------------------------------------------------------------------------*/
	/* For Card insert/remove                                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (intmask & SDHCI_INT_CARD_INSERT )
	{
        sdhci_writel(host, intmask & SDHCI_INT_CARD_INSERT, SDHCI_INT_STATUS);
        intmask &= ~SDHCI_INT_CARD_INSERT;

        SD_MSG("MMC0 Card Inserted ...  mmc rescan \n");
	}
	else if (intmask & SDHCI_INT_CARD_REMOVE)
	{
        sdhci_writel(host, intmask & SDHCI_INT_CARD_REMOVE, SDHCI_INT_STATUS);
        intmask &= ~SDHCI_INT_CARD_REMOVE;

        SD_MSG("MMC0 Card Removed ... \n");
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Show error message for the other interrupt(s).                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	if (intmask & SDHCI_INT_BUS_POWER)
	{
        printf("MMC0 Card is consuming too much power!\n");
        sdhci_writel(host, SDHCI_INT_BUS_POWER, SDHCI_INT_STATUS);
	}

	intmask &= ~SDHCI_INT_BUS_POWER;

	if (intmask)
	{
        printf("MMC0 Card Unexpected interrupt intmask = 0x%08x.\n", intmask);
        sdhci_dumpregs(host);
        sdhci_writel(host, intmask, SDHCI_INT_STATUS);
        result = HAL_ERROR;
	}

	return result;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sdhci2_irq                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles controller IRQs                                                   */
/*---------------------------------------------------------------------------------------------------------*/
static HAL_STATUS sdhci2_irq(UINT32 devNum)
{
	HAL_STATUS result = HAL_OK;;
	UINT32 intmask;
	struct sdhci_host *host = global_host[devNum];

	host->ioaddr = (void *)NPCMX50_PA_SDHC(devNum);
	intmask = sdhci_readl(host, SDHCI_INT_STATUS);

	/*-----------------------------------------------------------------------------------------------------*/
	/* If no interrupt or invalid status, ignore it.                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	if (!intmask || intmask == 0xffffffff)
	{
        SD_MSG("Got invalid interrupt2: 0x%08x\n", intmask);
        return result;
	}

	SD_MSG("Got interrupt2: intmask = 0x%08x\n", intmask);

	/*-----------------------------------------------------------------------------------------------------*/
	/* For Card insert/remove                                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (intmask & SDHCI_INT_CARD_INSERT )
	{
        sdhci_writel(host, intmask & SDHCI_INT_CARD_INSERT, SDHCI_INT_STATUS);
        intmask &= ~SDHCI_INT_CARD_INSERT;

        printf("MMC1 Card Inseretd ...  mmc rescan \n");
	}
	else if (intmask & SDHCI_INT_CARD_REMOVE)
	{
        sdhci_writel(host, intmask & SDHCI_INT_CARD_REMOVE, SDHCI_INT_STATUS);
        intmask &= ~SDHCI_INT_CARD_REMOVE;

        printf("MMC1 Card Removed ... \n");
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Show error message for the other interrupt(s).                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	if (intmask & SDHCI_INT_BUS_POWER)
	{
        printf("MMC1 Card is consuming too much power!\n");
        sdhci_writel(host, SDHCI_INT_BUS_POWER, SDHCI_INT_STATUS);
	}

	intmask &= ~SDHCI_INT_BUS_POWER;

	if (intmask)
	{
        printf("MMC1 Card Unexpected interrupt intmask = 0x%08x.\n", intmask);
        sdhci_dumpregs(host);
        sdhci_writel(host, intmask, SDHCI_INT_STATUS);
        result = HAL_ERROR;
	}

	return result;
}
#endif /* NPCM750 */

static void sdhci_reset(struct sdhci_host *host, u8 mask)
{
	unsigned long timeout;

	/* Wait max 100 ms */
	timeout = 100;
	sdhci_writeb(host, mask, SDHCI_SOFTWARE_RESET);

	/*-----------------------------------------------------------------------------------------------------*/
	/* If it is reset all, it must delay a while.                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	if (mask & SDHCI_RESET_ALL)
	{
        host->clock = 0;
        SD_MSG("reset host->clock = 0 for RESET_ALL\n");

        /*-------------------------------------------------------------------------------------------------*/
        /* Without the delay, the driver hangs after second reset called after registering IRQ, when the   */
        /* driver is built in kernel!                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
		udelay(1000);
	}

	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & mask) {
		if (timeout == 0) {
			printf("%s: Reset 0x%x never completed.\n",
			       __func__, (int)mask);
			sdhci_dumpregs(host);
			return;
		}
		timeout--;
		udelay(1000);
	}
}

static void sdhci_cmd_done(struct sdhci_host *host, struct mmc_cmd *cmd)
{
	int i;

	SD_MSG("slot = %d, opcode = %d, resp_type = %x\n", host->index, cmd->cmdidx, cmd->resp_type);
#ifdef DEBUG_SDHC
	sdhci_dumpregs(host);
#endif
	if (cmd->resp_type & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++) {
			cmd->response[i] = sdhci_readl(host,
					SDHCI_RESPONSE + (3-i)*4) << 8;
			if (i != 3)
				cmd->response[i] |= sdhci_readb(host,
						SDHCI_RESPONSE + (3-i)*4-1);
		}
	} else {
		cmd->response[0] = sdhci_readl(host, SDHCI_RESPONSE);
	}
}

static void sdhci_transfer_pio(struct sdhci_host *host, struct mmc_data *data)
{
	int i;
	char *offs;
	for (i = 0; i < data->blocksize; i += 4) {
		offs = data->dest + i;
		if (data->flags == MMC_DATA_READ)
			*(u32 *)offs = sdhci_readl(host, SDHCI_BUFFER);
		else
			sdhci_writel(host, *(u32 *)offs, SDHCI_BUFFER);
	}
}

static int sdhci_transfer_data(struct sdhci_host *host, struct mmc_data *data,
				unsigned int start_addr)
{
	unsigned int stat, rdy, mask, timeout, block = 0;
#ifdef CONFIG_MMC_SDMA
	unsigned char ctrl;
	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_DMA_MASK;
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
#endif

	timeout = 1000000;
	rdy = SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_AVAIL;
	mask = SDHCI_DATA_AVAILABLE | SDHCI_SPACE_AVAILABLE;
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			printf("%s: Error detected in status(0x%X)!\n",
			       __func__, stat);
			return -1;
		}
		if (stat & rdy) {
			if (!(sdhci_readl(host, SDHCI_PRESENT_STATE) & mask))
				continue;
			sdhci_writel(host, rdy, SDHCI_INT_STATUS);
			sdhci_transfer_pio(host, data);
			data->dest += data->blocksize;
			if (++block >= data->blocks)
				break;
		}
#ifdef CONFIG_MMC_SDMA
		if (stat & SDHCI_INT_DMA_END) {
			sdhci_writel(host, SDHCI_INT_DMA_END, SDHCI_INT_STATUS);
			start_addr &= ~(SDHCI_DEFAULT_BOUNDARY_SIZE - 1);
			start_addr += SDHCI_DEFAULT_BOUNDARY_SIZE;
			sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
		}
#endif
		if (timeout-- > 0)
			udelay(10);
		else {
			printf("%s: Transfer data timeout\n", __func__);
			return -1;
		}
	} while (!(stat & SDHCI_INT_DATA_END));
	return 0;
}

/*
	* No command will be sent by driver if card is busy, so driver must wait
	* for card ready state.
	* Every time when card is busy after timeout then (last) timeout value will be
	* increased twice but only if it doesn't exceed global defined maximum.
	* Each function call will use last timeout value. Max timeout can be redefined
	* in board config file.
	*/
#ifndef CONFIG_SDHCI_CMD_MAX_TIMEOUT
#define CONFIG_SDHCI_CMD_MAX_TIMEOUT		3200
#endif
#define CONFIG_SDHCI_CMD_DEFAULT_TIMEOUT	100

static int sdhci_send_command(struct mmc *mmc, struct mmc_cmd *cmd,
		       struct mmc_data *data)
{
	struct sdhci_host *host = mmc->priv;
	unsigned int stat = 0;
	int ret = 0;
	int trans_bytes = 0, is_aligned = 1;
	u32 mask, flags, mode;
	unsigned int time = 0, start_addr = 0;
	int mmc_dev = mmc->block_dev.dev;
	unsigned start = get_timer(0);

	/* Timeout unit - ms */
	static unsigned int cmd_timeout = CONFIG_SDHCI_CMD_DEFAULT_TIMEOUT;

	SD_MSG("slot = %d, opcode = %d, argument = %x\n", host->index, cmd->cmdidx, cmd->cmdarg);

	sdhci_activate_led(host);

	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	mask = SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT;

	/* We shouldn't wait for data inihibit for stop commands, even
	   though they might use busy signaling */
	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		mask &= ~SDHCI_DATA_INHIBIT;

	SD_MSG("slot = %d cmdidx = 0x%x mask=0x%x\n",host->index, cmd->cmdidx, mask);

	while (sdhci_readl(host, SDHCI_PRESENT_STATE) & mask) {
		if (time >= cmd_timeout) {
			printf("%s: MMC: %d busy ", __func__, mmc_dev);
			if (2 * cmd_timeout <= CONFIG_SDHCI_CMD_MAX_TIMEOUT) {
				cmd_timeout += cmd_timeout;
				printf("timeout increasing to: %u ms.\n",
				       cmd_timeout);
			} else {
                sdhci_deactivate_led(host);
				puts("timeout.\n");
				return COMM_ERR;
			}
		}
		time++;
		udelay(1000);
	}

	SD_MSG("slot = %d time = %d \n",host->index, time);

	mask = SDHCI_INT_RESPONSE;
	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags = SDHCI_CMD_RESP_NONE;
	else if (cmd->resp_type & MMC_RSP_136)
		flags = SDHCI_CMD_RESP_LONG;
	else if (cmd->resp_type & MMC_RSP_BUSY) {
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
		mask |= SDHCI_INT_DATA_END;
	} else
		flags = SDHCI_CMD_RESP_SHORT;

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= SDHCI_CMD_CRC;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= SDHCI_CMD_INDEX;
	if (data)
		flags |= SDHCI_CMD_DATA;

	/* Set Transfer mode regarding to data flag */
	if (data != 0) {
		sdhci_writeb(host, 0xe, SDHCI_TIMEOUT_CONTROL);
		mode = SDHCI_TRNS_BLK_CNT_EN;
		trans_bytes = data->blocks * data->blocksize;
		if (data->blocks > 1)
			mode |= SDHCI_TRNS_MULTI;

		if (data->flags == MMC_DATA_READ)
			mode |= SDHCI_TRNS_READ;

#ifdef CONFIG_MMC_SDMA
		if (data->flags == MMC_DATA_READ)
			start_addr = (unsigned long)data->dest;
		else
			start_addr = (unsigned long)data->src;
		if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) &&
				(start_addr & 0x7) != 0x0) {
			is_aligned = 0;
			start_addr = (unsigned long)aligned_buffer;
			if (data->flags != MMC_DATA_READ)
				memcpy(aligned_buffer, data->src, trans_bytes);
		}

#if defined(CONFIG_FIXED_SDHCI_ALIGNED_BUFFER)
		/*
		 * Always use this bounce-buffer when
		 * CONFIG_FIXED_SDHCI_ALIGNED_BUFFER is defined
		 */
		is_aligned = 0;
		start_addr = (unsigned long)aligned_buffer;
		if (data->flags != MMC_DATA_READ)
			memcpy(aligned_buffer, data->src, trans_bytes);
#endif

		sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
		mode |= SDHCI_TRNS_DMA;
#endif
		sdhci_writew(host, SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG,
				data->blocksize),
				SDHCI_BLOCK_SIZE);
		sdhci_writew(host, data->blocks, SDHCI_BLOCK_COUNT);
		sdhci_writew(host, mode, SDHCI_TRANSFER_MODE);
	} else if (cmd->resp_type & MMC_RSP_BUSY) {
		sdhci_writeb(host, 0xe, SDHCI_TIMEOUT_CONTROL);
	}

	sdhci_writel(host, cmd->cmdarg, SDHCI_ARGUMENT);
#ifdef CONFIG_MMC_SDMA
	flush_cache(start_addr, trans_bytes);
#endif
	sdhci_writew(host, SDHCI_MAKE_CMD(cmd->cmdidx, flags), SDHCI_COMMAND);
	start = get_timer(0);

#ifdef DEBUG_SDHC
	sdhci_dumpregs(host);
#endif
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR)
			break;
	} while (((stat & mask) != mask) &&
		 (get_timer(start) < CONFIG_SDHCI_CMD_DEFAULT_TIMEOUT));

	if (get_timer(start) >= CONFIG_SDHCI_CMD_DEFAULT_TIMEOUT) {
		if (host->quirks & SDHCI_QUIRK_BROKEN_R1B)
			return 0;
		else {
			printf("%s: Timeout for status update!\n", __func__);
			return TIMEOUT;
		}
	}

	if ((stat & (SDHCI_INT_ERROR | mask)) == mask) {
		sdhci_cmd_done(host, cmd);
		sdhci_writel(host, mask, SDHCI_INT_STATUS);
	} else
		ret = -1;

	if (!ret && data)
		ret = sdhci_transfer_data(host, data, start_addr);

	if (host->quirks & SDHCI_QUIRK_WAIT_SEND_CMD)
		udelay(1000);

	stat = sdhci_readl(host, SDHCI_INT_STATUS);
	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	if (!ret) {
		if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) &&
				!is_aligned && (data->flags == MMC_DATA_READ))
	    memcpy(data->dest, aligned_buffer, trans_bytes);
		sdhci_deactivate_led(host);
		return 0;
	}
	sdhci_deactivate_led(host);
	sdhci_reset(host, SDHCI_RESET_CMD);
	sdhci_reset(host, SDHCI_RESET_DATA);
	if (stat & SDHCI_INT_TIMEOUT)
	{
	    DBG_MSG("CMD TIMEOUT slot=%d cmd=%d \n",host->index, cmd->cmdidx);
		return TIMEOUT;
	}
	else
	{
	    DBG_MSG("CMD ERROR slot=%d cmd=%d \n",host->index, cmd->cmdidx);
		return COMM_ERR;
	}
}

static int sdhci_set_clock(struct mmc *mmc, unsigned int clock)
{
	struct sdhci_host *host = mmc->priv;
	unsigned int div, clk, timeout;

	SD_MSG("slot = %d, host->clock = %d, clock = %d\n",
            host->index, host->clock, clock);

	/*-----------------------------------------------------------------------------------------------------*/
	/* f the desired clock equals to the clock set in host, just return.                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	if (clock == host->clock)
        return 0;

#ifndef NPCM750
	/*-----------------------------------------------------------------------------------------------------*/
	/* If it is MMC, must disable SD sampling delay.                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	if (clock == 26000000 || clock == 20000000)
	{
        clear_bit(NPCMX50_SDSD,(void *)(NPCMX50_INTCR2));
	}
#endif
	/*-----------------------------------------------------------------------------------------------------*/
	/* Set clock to 0 first.                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	sdhci_writew(host, 0, SDHCI_CLOCK_CONTROL);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable SD sampling delay for SD standard timing if clock=0                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	if (clock == 0)
		return 0;

	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		/* Version 3.00 divisors must be a multiple of 2. */
		if (mmc->cfg->f_max <= clock)
			div = 1;
		else {
			for (div = 2; div < SDHCI_MAX_DIV_SPEC_300; div += 2) {
				if ((mmc->cfg->f_max / div) <= clock)
					break;
			}
		}
	} else {
		/* Version 2.00 divisors must be a power of 2. */
		for (div = 1; div < SDHCI_MAX_DIV_SPEC_200; div *= 2) {
			if ((mmc->cfg->f_max / div) <= clock)
				break;
		}
	}
	div >>= 1;

	if (host->set_clock)
		host->set_clock(host->index, div);

	clk = (div & SDHCI_DIV_MASK) << SDHCI_DIVIDER_SHIFT;
	clk |= ((div & SDHCI_DIV_HI_MASK) >> SDHCI_DIV_MASK_LEN)
		<< SDHCI_DIVIDER_HI_SHIFT;
	clk |= SDHCI_CLOCK_INT_EN;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set up register for the clock.                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait until the clock is stable, or max 20 ms                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	timeout = 20;
	while (!((clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL))
		& SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			printf("%s: Internal clock never stabilised.\n",
			       __func__);
            sdhci_dumpregs(host);
			return -1;
		}
		timeout--;
		udelay(1000);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable the clock.                                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	clk |= SDHCI_CLOCK_CARD_EN;

	SD_MSG("SDHCI_CLOCK_CONTROL clk = 0x%x\n", clk);

	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

	return 0;
}

static void sdhci_set_power(struct sdhci_host *host, unsigned short power)
{
	u8 pwr = 0;


	SD_MSG(" power = 0x%x\n", power);

	if (power != (unsigned short)-1) {
		switch (1 << power) {
		case MMC_VDD_165_195:
			pwr = SDHCI_POWER_180;
			break;
		case MMC_VDD_29_30:
		case MMC_VDD_30_31:
			pwr = SDHCI_POWER_300;
			break;
		case MMC_VDD_32_33:
		case MMC_VDD_33_34:
			pwr = SDHCI_POWER_330;
			break;
		}
	}

	SD_MSG("SDHCI_POWER_CONTROL pwr = 0x%x\n", pwr);

	if (pwr == 0) {
		sdhci_writeb(host, 0, SDHCI_POWER_CONTROL);
	    sdhci_reset(host, SDHCI_RESET_ALL);
		return;
	}

	if (host->quirks & SDHCI_QUIRK_NO_SIMULT_VDD_AND_POWER)
		sdhci_writeb(host, pwr, SDHCI_POWER_CONTROL);

	pwr |= SDHCI_POWER_ON;

	sdhci_writeb(host, pwr, SDHCI_POWER_CONTROL);
}

static void sdhci_set_ios(struct mmc *mmc)
{
	u32 ctrl;
	struct sdhci_host *host = mmc->priv;

	SD_MSG("slot = %d, mmc->clock=%d mmc->bus_width=%d\n",
           host->index, mmc->clock, mmc->bus_width);


	if (host->set_control_reg)
		host->set_control_reg(host);

	if (mmc->clock != host->clock)
		sdhci_set_clock(mmc, mmc->clock);

	/* Set bus width */
	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	if (mmc->bus_width == 8) {
		ctrl &= ~SDHCI_CTRL_4BITBUS;
		if ((SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) ||
				(host->quirks & SDHCI_QUIRK_USE_WIDE8))
			ctrl |= SDHCI_CTRL_8BITBUS;
	} else {
		if ((SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) ||
				(host->quirks & SDHCI_QUIRK_USE_WIDE8))
			ctrl &= ~SDHCI_CTRL_8BITBUS;
		if (mmc->bus_width == 4)
			ctrl |= SDHCI_CTRL_4BITBUS;
		else
			ctrl &= ~SDHCI_CTRL_4BITBUS;
	}

	if (mmc->clock > 26000000)
		ctrl |= SDHCI_CTRL_HISPD;
	else
		ctrl &= ~SDHCI_CTRL_HISPD;

	if (host->quirks & SDHCI_QUIRK_NO_HISPD_BIT)
		ctrl &= ~SDHCI_CTRL_HISPD;

	SD_MSG("SDHCI_HOST_CONTROL ctrl = %d\n", ctrl);

	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
}

static int sdhci_init(struct mmc *mmc)
{
	struct sdhci_host *host = mmc->priv;

	sdhci_reset(host, SDHCI_RESET_ALL);

	if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) && !aligned_buffer) {
		aligned_buffer = memalign(8, 512*1024);
		if (!aligned_buffer) {
			printf("%s: Aligned buffer alloc failed!!!\n",
			       __func__);
			return -1;
		}
	}

	sdhci_set_power(host, fls(mmc->cfg->voltages) - 1);

	if (host->quirks & SDHCI_QUIRK_NO_CD) {
		unsigned int status;

		sdhci_writeb(host, SDHCI_CTRL_CD_TEST_INS | SDHCI_CTRL_CD_TEST,
			SDHCI_HOST_CONTROL);

		status = sdhci_readl(host, SDHCI_PRESENT_STATE);
		while ((!(status & SDHCI_CARD_PRESENT)) ||
		    (!(status & SDHCI_CARD_STATE_STABLE)) ||
		    (!(status & SDHCI_CARD_DETECT_PIN_LEVEL)))
			status = sdhci_readl(host, SDHCI_PRESENT_STATE);
	}

#ifdef SDHC_INTERRUPT
	/* Enable only interrupts served by the SD controller */
	sdhci_writel(host, SDHCI_INT_CARD_INSERT | SDHCI_INT_CARD_REMOVE | SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK, SDHCI_INT_ENABLE);
	/* Mask all sdhci interrupt sources */
	sdhci_writel(host, SDHCI_INT_CARD_INSERT | SDHCI_INT_CARD_REMOVE, SDHCI_SIGNAL_ENABLE);
#else
	/* Enable only interrupts served by the SD controller */
	sdhci_writel(host, SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK,
		     SDHCI_INT_ENABLE);
	/* Mask all sdhci interrupt sources */
	sdhci_writel(host, 0x0, SDHCI_SIGNAL_ENABLE);

#endif
	return 0;
}


static const struct mmc_ops sdhci_ops = {
	.send_cmd	= sdhci_send_command,
	.set_ios	= sdhci_set_ios,
	.init		= sdhci_init,
};

int add_sdhci(struct sdhci_host *host, u32 max_clk, u32 min_clk)
{
	unsigned int caps;

	host->cfg.name = host->name;
	host->cfg.ops = &sdhci_ops;

	caps = sdhci_readl(host, SDHCI_CAPABILITIES);
#ifdef CONFIG_MMC_SDMA
	if (!(caps & SDHCI_CAN_DO_SDMA)) {
		printf("%s: Your controller doesn't support SDMA!!\n",
		       __func__);
		return -1;
	}
#endif

	if (max_clk)
		host->cfg.f_max = max_clk;
	else {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			host->cfg.f_max = (caps & SDHCI_CLOCK_V3_BASE_MASK)
				>> SDHCI_CLOCK_BASE_SHIFT;
		else
			host->cfg.f_max = (caps & SDHCI_CLOCK_BASE_MASK)
				>> SDHCI_CLOCK_BASE_SHIFT;
		host->cfg.f_max *= 1000000;
	}
	if (host->cfg.f_max == 0) {
		printf("%s: Hardware doesn't specify base clock frequency\n",
		       __func__);
		return -1;
	}
	if (min_clk)
		host->cfg.f_min = min_clk;
	else {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			host->cfg.f_min = host->cfg.f_max /
				SDHCI_MAX_DIV_SPEC_300;
		else
			host->cfg.f_min = host->cfg.f_max /
				SDHCI_MAX_DIV_SPEC_200;
	}

	host->cfg.voltages = 0;
	if (caps & SDHCI_CAN_VDD_330)
		host->cfg.voltages |= MMC_VDD_32_33 | MMC_VDD_33_34;
	if (caps & SDHCI_CAN_VDD_300)
		host->cfg.voltages |= MMC_VDD_29_30 | MMC_VDD_30_31;
	if (caps & SDHCI_CAN_VDD_180)
		host->cfg.voltages |= MMC_VDD_165_195;

	if (host->quirks & SDHCI_QUIRK_BROKEN_VOLTAGE)
		host->cfg.voltages |= host->voltages;

	host->cfg.host_caps = MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_4BIT;
	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		if (caps & SDHCI_CAN_DO_8BIT)
			host->cfg.host_caps |= MMC_MODE_8BIT;
	}
	if (host->host_caps)
		host->cfg.host_caps |= host->host_caps;

	SD_MSG("host->cfg.host_caps = 0x%x\n", host->cfg.host_caps);

	host->cfg.b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	sdhci_reset(host, SDHCI_RESET_ALL);

	host->mmc = mmc_create(&host->cfg, host);
	if (host->mmc == NULL) {
		printf("%s: mmc create fail!\n", __func__);
		return -1;
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                         Device init                                          */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sdhci_init_slot                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                               */
/*                  slot -                                                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs slot initialization                                              */
/*---------------------------------------------------------------------------------------------------------*/
static int sdhci_init_slot(int slot)
{
	int ret = 0;
	unsigned int version;
	struct sdhci_host *host = NULL;

	host = (struct sdhci_host *)calloc(1,sizeof(struct sdhci_host));
	if (!host) {
		printf("sdhci_init_slot: sdhci_host calloc fail\n");
		return 1;
	}
	global_host[slot] = host;


	host->name = "npcmx50_sdhci";
	host->ioaddr = (void *)NPCMX50_PA_SDHC(slot);
	host->quirks = SDHCI_QUIRK_BROKEN_R1B | SDHCI_QUIRK_WAIT_SEND_CMD;
	if(slot == SD2_DEV)
	{
	   host->quirks |= SDHCI_QUIRK_NO_CD;     /* eMMC - Set Software Card Detect */
	}
	host->index = slot;

	sdhci_reset(host, SDHCI_RESET_ALL);

	/*-------------------------------------------------------------------------------------------------*/
	/* Read version and capabilities of the SDHC.                                                      */
	/*-------------------------------------------------------------------------------------------------*/
	version = sdhci_readw(host, SDHCI_HOST_VERSION);
	host->version = version = (version & SDHCI_SPEC_VER_MASK) >> SDHCI_SPEC_VER_SHIFT;
	SD_MSG("SDHC version = %d\n", version);
	if (version > 2)
	{
        printf("NPCMX50 SD/MMC slot=%d: Unknown controller version (%d). "
                "You may experience problems.\n", host->index,
                version);
	}

/*-----------------------------------------------------------------------------------------------------*/
/* Enable Interrupt Group                                                                              */
/*-----------------------------------------------------------------------------------------------------*/
#ifdef SDHC_INTERRUPT
#if defined (SD_INTERRUPT)
	if(slot == SD1_DEV)
	{
       ret |= AIC_RegisterHandler(SD_INTERRUPT(slot), sdhci1_irq, (UINT32)slot);
	}
	else if(slot == SD2_DEV)
	{
       ret |= AIC_RegisterHandler(SD_INTERRUPT(slot), sdhci2_irq, (UINT32)slot);
	}
	ret |= AIC_EnableInt(SD_INTERRUPT(slot));
#endif
#endif

	add_sdhci(host, 0, 0);

	SD_MSG("caps = 0x%x, f_max = %d, f_min = %d\n",
            sdhci_readl(host, SDHCI_CAPABILITIES), host->cfg.f_max, host->cfg.f_min);

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              Hardware init                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sdhci_module_init                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs HW init                                                          */
/*---------------------------------------------------------------------------------------------------------*/
static void sdhci_module_init(UINT32 sdNum)
{
	SD_MSG("enter\n");

#ifdef SDHC_INTERRUPT
#if defined (SD_INTERRUPT)
	AIC_DisableInt(SD_INTERRUPT(sdNum));
#endif
#endif

	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable SDIO                                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	if(SD1_DEV == sdNum)
	{
		GCR_Mux_SD(sdNum, SD_DEV_SD);
	}
	else if(SD2_DEV == sdNum)
	{
		GCR_Mux_SD(sdNum, SD_DEV_MMC8);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* SD Card Interface - Set/clear Software Reset Control Bit                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_ResetSD(sdNum);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        npcmx50_mmc_init                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
int npcmx50_mmc_init(void)
{
	int ret=0;
	int i;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initialize registers for SDHC                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	sdhci_module_init(SD1_DEV);
	sdhci_module_init(SD2_DEV);

#ifdef _PALLADIUM_
	printf("Palladium - Wait few minutes for SD card to be detected !!!\n");
#endif

	for (i = 0; i < NUM_SLOTS; i++)
	{
        ret = sdhci_init_slot(i);
        if (ret)
        {
            printf("NPCMX50 SD/MMC npcmx50_mmc_init error, slot %d\n", i);
        }
	}
	return ret;
}