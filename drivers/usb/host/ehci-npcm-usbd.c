// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gcr.h>
#include "ehci.h"

struct npcm_usbd_priv {
	struct ehci_ctrl ctrl;
	struct ehci_hccr *hccr;
	u32 phy_num;
	u32 dev_num;
};

static int npcm_usbd_of_to_plat(struct udevice *dev)
{
	struct npcm_usbd_priv *priv = dev_get_priv(dev);
	u32 phy_num, dev_num;

	phy_num = dev_read_u32_default(dev, "usbphy", 1);
	dev_num = dev_read_u32_default(dev, "usbdev", 0);

	/*
	 * phy1: device 0 ~ 9
	 * phy2: device 9
	 * phy3: device 8 (for NPCM8xx)
	 */
	switch (phy_num) {
	case 1:
		if (dev_num > 9)
			return -ENXIO;
		break;
	case 2:
		if (dev_num != 9)
			return -ENXIO;
		break;
	case 3:
		if (!IS_ENABLED(CONFIG_ARCH_NPCM8XX))
			return -ENXIO;
		if (dev_num != 8)
			return -ENXIO;
		break;
	default:
		return -ENXIO;
	}
	priv->phy_num = phy_num;
	priv->dev_num = dev_num;
	priv->hccr = dev_read_addr_ptr(dev) + (0x1000 * dev_num);

	printf("USB Device Phy-%d Device-%d at Address 0x%p\n",
	       priv->phy_num, priv->dev_num, priv->hccr);

	return 0;
}

static int npcm_usbd_probe(struct udevice *dev)
{
	struct npcm_usbd_priv *priv = dev_get_priv(dev);
	struct ehci_hcor *hcor;
	struct npcm_gcr *gcr = (struct npcm_gcr *)npcm_get_base_gcr();

	if (IS_ENABLED(CONFIG_ARCH_NPCM8XX) && priv->dev_num == 8) {
		if (priv->phy_num == 1)
			writel((readl(&gcr->intcr3) & ~(0x3 << INTCR3_USBPHY3SW)),
			       &gcr->intcr3);
		else if (priv->phy_num == 3)
			writel((readl(&gcr->intcr3) | (0x3 << INTCR3_USBPHY3SW)),
			       &gcr->intcr3);
	}
	if (priv->dev_num == 9) {
		if (priv->phy_num == 1)
			writel((readl(&gcr->intcr3) & ~(0x3 << INTCR3_USBPHY2SW)),
			       &gcr->intcr3);
		else if (priv->phy_num == 2)
			writel((readl(&gcr->intcr3) | (0x3 << INTCR3_USBPHY2SW)),
			       &gcr->intcr3);
	}

	hcor = (struct ehci_hcor *)((uintptr_t)priv->hccr +
			HC_LENGTH(ehci_readl(&priv->hccr->cr_capbase)));

	return ehci_register(dev, priv->hccr, hcor, NULL, 0, USB_INIT_DEVICE);
}

static int npcm_usbd_remove(struct udevice *dev)
{
	struct npcm_usbd_priv *priv = dev_get_priv(dev);
	struct npcm_gcr *gcr = (struct npcm_gcr *)npcm_get_base_gcr();

	printf("%s: Phy-%d\n", __func__, priv->phy_num);
	if (priv->phy_num == 2)
		writel(((readl(&gcr->intcr3) & ~(0x3 << INTCR3_USBPHY2SW))
		       | (0x1 << INTCR3_USBPHY2SW)), &gcr->intcr3);
	if (IS_ENABLED(CONFIG_ARCH_NPCM8XX) && priv->phy_num == 3)
		writel(((readl(&gcr->intcr3) & ~(0x3 << INTCR3_USBPHY3SW))
		       | (0x1 << INTCR3_USBPHY3SW)), &gcr->intcr3);

	return ehci_deregister(dev);
}

static const struct udevice_id npcm_usbd_ids[] = {
	{ .compatible = "nuvoton,npcm845-usbd" },
	{ .compatible = "nuvoton,npcm750-usbd" },
	{ }
};

U_BOOT_DRIVER(usbd_npcm) = {
	.name	= "npcm_usbd",
	.id	= UCLASS_USB,
	.of_match = npcm_usbd_ids,
	.of_to_plat = npcm_usbd_of_to_plat,
	.probe = npcm_usbd_probe,
	.remove = npcm_usbd_remove,
	.ops	= &ehci_usb_ops,
	.priv_auto = sizeof(struct npcm_usbd_priv),
	.plat_auto = sizeof(struct usb_plat),
	.flags	= DM_FLAG_ALLOC_PRIV_DMA,
};
