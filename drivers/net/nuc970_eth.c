/*
	* Copyright (c) 2014 Nuvoton Technology Corp.
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
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 59 Temple Place, Suite 330, Boston,
	* MA 02111-1307 USA
	*
	* Description:   NUC970 MAC driver source file
	*/

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <miiphy.h>
#include <malloc.h>
#include <linux/mii.h>
#include <linux/err.h>
#include "nuc970_eth.h"

#define NUC_MSG(fmt,args...) printf ("EMC: %s(): " fmt ,__func__, ##args)

/*#define VERBOSE_EMC */
#if defined(VERBOSE_GLOBAL) || defined(VERBOSE_EMC)
#define NUC_DEBUG_MSG(fmt,args...) NUC_MSG(fmt ,##args)
#else
#define NUC_DEBUG_MSG(fmt,args...)
#endif


static int nuc970_eth_mii_write(struct nuc970_eth_dev *priv, uchar addr, uchar reg, ushort val)
{

        writel(val, priv->iobase + MIID);
        writel((addr << 8) | reg | PHYBUSY | PHYWR | MDCCR, priv->iobase + MIIDA);

        while (readl(priv->iobase + MIIDA) & PHYBUSY);

        return(0);
}


static int nuc970_eth_mii_read(struct nuc970_eth_dev *priv, uchar addr, uchar reg, ushort *val)
{
        writel((addr << 8) | reg | PHYBUSY | MDCCR, priv->iobase + MIIDA);
        while (readl(priv->iobase + MIIDA) & PHYBUSY);

        *val = (ushort)readl(priv->iobase + MIID);

        return(0);
}

static int nuc970_reset_phy(struct nuc970_eth_dev *priv)
{
	unsigned short reg;
	int delay;

	NUC_DEBUG_MSG("\n");

	nuc970_eth_mii_write(priv, CONFIG_NUC970_PHY_ADDR, MII_BMCR, BMCR_RESET);

	delay = 2000;
	while(delay-- > 0) {
            nuc970_eth_mii_read(priv, CONFIG_NUC970_PHY_ADDR, MII_BMCR, &reg);
            if((reg & BMCR_RESET) == 0)
                    break;

	}

	if(delay == 0) {
            NUC_MSG("Reset phy failed\n");
            return(-1);
	}

	nuc970_eth_mii_write(priv, CONFIG_NUC970_PHY_ADDR, MII_ADVERTISE, ADVERTISE_CSMA |
                                                                ADVERTISE_10HALF |
                                                                ADVERTISE_10FULL |
                                                                ADVERTISE_100HALF |
                                                                ADVERTISE_100FULL);

	nuc970_eth_mii_read(priv, CONFIG_NUC970_PHY_ADDR, MII_BMCR, &reg);
	nuc970_eth_mii_write(priv, CONFIG_NUC970_PHY_ADDR, MII_BMCR, reg | BMCR_ANRESTART);

	delay = 20000;
	while(delay-- > 0) {
            nuc970_eth_mii_read(priv, CONFIG_NUC970_PHY_ADDR, MII_BMSR, &reg);
            if((reg & (BMSR_ANEGCOMPLETE | BMSR_LSTATUS)) == (BMSR_ANEGCOMPLETE | BMSR_LSTATUS))
                    break;
	}

	if(delay == 0) {
            NUC_MSG("AN failed. Set to 100 FULL\n");
            writel(readl(priv->iobase + MCMDR) | MCMDR_OPMOD | MCMDR_FDUP, priv->iobase + MCMDR);
            return(-1);
	} else {
            nuc970_eth_mii_read(priv, CONFIG_NUC970_PHY_ADDR, MII_LPA, &reg);

            if(reg | ADVERTISE_100FULL)
                    writel(readl(priv->iobase + MCMDR) | MCMDR_OPMOD | MCMDR_FDUP, priv->iobase + MCMDR);
            else if(reg | ADVERTISE_100HALF)
                    writel((readl(priv->iobase + MCMDR) | MCMDR_OPMOD) & ~MCMDR_FDUP, priv->iobase + MCMDR);
            else if(reg | ADVERTISE_10FULL)
                    writel((readl(priv->iobase + MCMDR) | MCMDR_FDUP) & ~MCMDR_OPMOD, priv->iobase + MCMDR);
            else
                    writel(readl(priv->iobase + MCMDR) & ~(MCMDR_OPMOD | MCMDR_FDUP), priv->iobase + MCMDR);
	}

	return(0);
}


static void init_tx_desc(struct nuc970_eth_dev *priv)
{
        int i;
        struct eth_descriptor *tx_desc = priv->tx_desc;

        do {
            writel((unsigned int)(&tx_desc[0]), priv->iobase + TXDLSA);
            NUC_DEBUG_MSG("TXDLSA = %08X %08X\n", (unsigned int)(&tx_desc[0]), readl(priv->iobase + TXDLSA));
        } while (readl(priv->iobase + TXDLSA) != (unsigned int)(&tx_desc[0]));

        /*priv->tx_desc_ptr = &tx_desc[0]; */
        priv->tx_index = 0;

        for(i = 0; i < TX_DESCRIPTOR_NUM; i++) {
                tx_desc[i].status1 = PaddingMode | CRCMode/* | MACTxIntEn*/;
                tx_desc[i].buf = (unsigned char*)&(priv->txbuffs[i]);
                tx_desc[i].status2 = 0;
                tx_desc[i].next = (struct eth_descriptor*)((unsigned int) &tx_desc[(i + 1) % TX_DESCRIPTOR_NUM]);
                NUC_DEBUG_MSG("tx_desc= %p: %08X, %p %08X, %p\n", &tx_desc[i], tx_desc[i].status1, tx_desc[i].buf, tx_desc[i].status2, tx_desc[i].next);
        }

        flush_dcache_range((unsigned int)(&tx_desc[0]),(unsigned int)(&tx_desc[TX_DESCRIPTOR_NUM]));

        return;
}

static void init_rx_desc(struct nuc970_eth_dev *priv)
{
	int i;
	struct eth_descriptor *rx_desc = priv->rx_desc;


	/* Before passing buffers to GMAC we need to make sure zeros
	* written there right after "priv" structure allocation were
	* flushed into RAM.
	* Otherwise there's a chance to get some of them flushed in RAM when
	* GMAC is already pushing data to RAM via DMA. This way incoming from
	* GMAC data will be corrupted. */
	flush_dcache_range((unsigned int)priv->rxbuffs[0], (unsigned int)priv->rxbuffs[RX_DESCRIPTOR_NUM]);

	writel((unsigned int)&rx_desc[0], priv->iobase + RXDLSA);
	priv->rx_index = 0;

	for(i = 0; i < RX_DESCRIPTOR_NUM; i++) {
            rx_desc[i].status1 = RXfOwnership_DMA;
            rx_desc[i].buf = priv->rxbuffs[i];
            rx_desc[i].status2 = 0;
            rx_desc[i].next = (struct eth_descriptor *)&rx_desc[(i + 1) % RX_DESCRIPTOR_NUM];
            NUC_DEBUG_MSG("rx_desc= %p: %08X, %p %08X, %p\n", &rx_desc[i], rx_desc[i].status1, rx_desc[i].buf, rx_desc[i].status2, rx_desc[i].next);
	}

	flush_dcache_range((unsigned int)(&rx_desc[0]),(unsigned int)(&rx_desc[RX_DESCRIPTOR_NUM]));

	return;
}

static int nuc970_eth_write_hwaddr(struct eth_device *dev)
{
	struct nuc970_eth_dev *priv = dev->priv;

	NUC_DEBUG_MSG("\n");
	writel((dev->enetaddr[0] << 24) |
           (dev->enetaddr[1] << 16) |
           (dev->enetaddr[2] << 8) |
           dev->enetaddr[3] , priv->iobase + CAM0M);

	writel((dev->enetaddr[4] << 24) |
           (dev->enetaddr[5] << 16) , priv->iobase + CAM0L);

	/*writel(CAM_ECMP | CAM_AUP, priv->iobase + CAMCMR); */
	writel(CAM_ECMP | CAM_AUP | CAM_ABP, priv->iobase + CAMCMR);
        writel(1, priv->iobase + CAMEN);
        return(0);
}


static int nuc970_eth_init(struct eth_device *dev, bd_t *bis)
{
	struct nuc970_eth_dev *priv = dev->priv;

	NUC_DEBUG_MSG("\n");
	/* Reset MAC */
	writel(MCMDR_SWR, priv->iobase + MCMDR);
	while(readl(priv->iobase + MCMDR) & MCMDR_SWR);


	init_tx_desc(priv);
	init_rx_desc(priv);

	nuc970_eth_write_hwaddr(dev);  /* need to reconfigure hardware address 'cos we just RESET emc... */

	writel(MCMDR_SPCRC | MCMDR_RXON | MCMDR_EnMDC | MCMDR_TXON, priv->iobase + MCMDR);
	writel(0, priv->iobase + RSDR);

	return(nuc970_reset_phy(priv));

}



static void  nuc970_eth_halt(struct eth_device *dev)
{
	struct nuc970_eth_dev *priv = dev->priv;

	NUC_DEBUG_MSG("\n");
	writel(readl(priv->iobase + MCMDR) & ~(MCMDR_RXON|MCMDR_TXON), priv->iobase + MCMDR);
}

static int nuc970_eth_recv (struct eth_device *dev)
{
	struct nuc970_eth_dev *priv = dev->priv;
	unsigned int rx_index = priv->rx_index;
	struct eth_descriptor *rx_desc_ptr = &(priv->rx_desc[rx_index]);

	while(1) {
		/* Invalidate entire buffer descriptor */
		invalidate_dcache_range((unsigned long)rx_desc_ptr, (unsigned long)(rx_desc_ptr+1));

        if(rx_desc_ptr->status1 & RXfOwnership_DMA)
            break;

        if(rx_desc_ptr->status1 & RXFD_RXGD) {
            invalidate_dcache_range((unsigned long)rx_desc_ptr->buf, (unsigned long)rx_desc_ptr->buf + roundup(rx_desc_ptr->status1 & 0xFFFF, ARCH_DMA_MINALIGN));
            net_process_received_packet(rx_desc_ptr->buf, rx_desc_ptr->status1 & 0xFFFF);
        }
        NUC_DEBUG_MSG("rx_desc_ptr= %p: %08X, %p %08X, %p\n", rx_desc_ptr, rx_desc_ptr->status1, rx_desc_ptr->buf, rx_desc_ptr->status2, rx_desc_ptr->next);

        rx_desc_ptr->status1 = RXfOwnership_DMA;
		/* Flush modified buffer descriptor */
		flush_dcache_range((unsigned long)rx_desc_ptr, (unsigned long)(rx_desc_ptr+1));

        rx_index = (rx_index+1) % RX_DESCRIPTOR_NUM;
        rx_desc_ptr = &(priv->rx_desc[rx_index]);
	}

	priv->rx_index = rx_index;

	writel(0, priv->iobase + RSDR);

	return 0;

}

static int nuc970_eth_send(struct eth_device *dev, void *packet, int length)
{
	struct nuc970_eth_dev *priv = dev->priv;
	unsigned int tx_index = priv->tx_index;
	struct eth_descriptor *tx_desc_ptr = &(priv->tx_desc[tx_index]);

	/*
	 * Strictly we only need to invalidate the "txrx_status" field
	 * for the following check, but on some platforms we cannot
	 * invalidate only 4 bytes, so we flush the entire descriptor,
	 * which is 16 bytes in total. This is safe because the
	 * individual descriptors in the array are each aligned to
	 * ARCH_DMA_MINALIGN and padded appropriately.
	 */
	/*invalidate_dcache_range((unsigned long)tx_desc_ptr, (unsigned long)(tx_desc_ptr+1)); */
	/* wait 'til transfer complete. (e.g. ownership again set to CPU) */
	do {
        invalidate_dcache_range((unsigned long)tx_desc_ptr, (unsigned long)(tx_desc_ptr+1));
	}
	while(tx_desc_ptr->status1 & TXfOwnership_DMA);

	memcpy(&(priv->txbuffs[tx_index]), packet, length);
	/* Flush data to be sent */
	flush_dcache_range((unsigned long)&(priv->txbuffs[tx_index]), (unsigned long)&(priv->txbuffs[tx_index])+roundup(length, ARCH_DMA_MINALIGN));

	tx_desc_ptr->status2 = (unsigned int)length;
	tx_desc_ptr->status1 |= TXfOwnership_DMA;

	/* Flush modified buffer descriptor */
	flush_dcache_range((unsigned long)tx_desc_ptr, (unsigned long)(tx_desc_ptr+1));

	writel(0, priv->iobase + TSDR);
	NUC_DEBUG_MSG("tx_desc_ptr= %p: %08X, %p %08X, %p\n", tx_desc_ptr, tx_desc_ptr->status1, tx_desc_ptr->buf, tx_desc_ptr->status2, tx_desc_ptr->next);

	priv->tx_index = (tx_index+1) % TX_DESCRIPTOR_NUM;

		return 0 ;
}




int nuc970_eth_register(ulong base_addr, unsigned int dev_num)
{
	struct eth_device *dev;
	struct nuc970_eth_dev *priv;
	NUC_DEBUG_MSG("\n");

	dev = malloc(sizeof(*dev));
	if (dev == NULL)
        return(-ENOMEM);

	/*
	 * Since the priv structure contains the descriptors which need a strict
	 * buswidth alignment, memalign is used to allocate memory
	 */
	priv = (struct nuc970_eth_dev *) memalign(MAX_ETH_BUFSIZE,
					      sizeof(struct nuc970_eth_dev));
	if (!priv) {
		free(dev);
		return -ENOMEM;
	}

	memset(dev, 0, sizeof(struct eth_device));
	memset(priv, 0, sizeof(struct nuc970_eth_dev));
	sprintf(dev->name, "ETH%d", dev_num);

	dev->iobase = (phys_addr_t)base_addr;
	dev->priv = priv;

	priv->iobase = (phys_addr_t)base_addr;

	dev->init = nuc970_eth_init;
	dev->halt = nuc970_eth_halt;
	dev->send = nuc970_eth_send;
	dev->recv = nuc970_eth_recv;
	dev->write_hwaddr = nuc970_eth_write_hwaddr;

	eth_register(dev);

	return(0);
}
