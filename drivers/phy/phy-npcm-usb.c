// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#include <common.h>
#include <dm.h>
#include <generic-phy.h>
#include <regmap.h>
#include <reset.h>
#include <syscon.h>
#include <dm/device_compat.h>
#include <linux/bitfield.h>
#include <linux/delay.h>

/* GCR Register Offsets */
#define GCR_INTCR3	0x9C
#define GCR_USB1PHYCTL	0x140
#define GCR_USB2PHYCTL	0x144
#define GCR_USB3PHYCTL	0x148

/* USBnPHYCTL bit fields */
#define PHYCTL_RS	BIT(28)

#define USBPHY2SW	GENMASK(13, 12)
#define USBPHY3SW	GENMASK(15, 14)

#define USBPHY2SW_DEV9_PHY1	FIELD_PREP(USBPHY2SW, 0)
#define USBPHY2SW_HOST1		FIELD_PREP(USBPHY2SW, 1)
#define USBPHY2SW_DEV9_PHY2	FIELD_PREP(USBPHY2SW, 3)
#define USBPHY3SW_DEV8_PHY1	FIELD_PREP(USBPHY3SW, 0)
#define USBPHY3SW_HOST2		FIELD_PREP(USBPHY3SW, 1)
#define USBPHY3SW_DEV8_PHY3	FIELD_PREP(USBPHY3SW, 3)

enum phy_id {
	USBDPHY1,	/* Dev 0~7 - PHY1 */
	USBD8PHY1,	/* Dev 8   - PHY1 */
	USBD8PHY3,	/* Dev 8   - PHY3 */
	USBD9PHY1,	/* Dev 9   - PHY1 */
	USBD9PHY2,	/* Dev 9   - PHY2 */
	USBH1PHY2,	/* Host1   - PHY2 */
	USBH2PHY3,	/* Host2   - PHY3 */
};

struct npcm_usbphy {
	struct regmap *gcr_syscon;
};

static int npcm_usb_phy_init(struct phy *phy)
{
	struct npcm_usbphy *priv = dev_get_priv(phy->dev);
	struct reset_ctl reset;
	int ret;

	ret = reset_get_by_index(phy->dev, 0, &reset);
	if (ret && ret != -ENOENT && ret != -ENOTSUPP) {
		dev_err(phy->dev, "can't get phy reset ctrl (err %d)", ret);
		return ret;
	}

	/* setup PHY switch */
	switch (phy->id) {
	case USBD8PHY1:
		regmap_update_bits(priv->gcr_syscon, GCR_INTCR3, USBPHY3SW,
				   USBPHY3SW_DEV8_PHY1);
		break;
	case USBD8PHY3:
		regmap_update_bits(priv->gcr_syscon, GCR_INTCR3, USBPHY3SW,
				   USBPHY3SW_DEV8_PHY3);
		break;
	case USBD9PHY1:
		regmap_update_bits(priv->gcr_syscon, GCR_INTCR3, USBPHY2SW,
				   USBPHY2SW_DEV9_PHY1);
		break;
	case USBD9PHY2:
		regmap_update_bits(priv->gcr_syscon, GCR_INTCR3, USBPHY2SW,
				   USBPHY2SW_DEV9_PHY2);
		break;
	case USBH1PHY2:
		regmap_update_bits(priv->gcr_syscon, GCR_INTCR3, USBPHY2SW,
				   USBPHY2SW_HOST1);
		break;
	case USBH2PHY3:
		regmap_update_bits(priv->gcr_syscon, GCR_INTCR3, USBPHY3SW,
				   USBPHY3SW_HOST2);
		break;
	default:
		break;
	}
	/* reset phy */
	if (reset_valid(&reset))
		reset_assert(&reset);

	/* Wait for PHY clocks to stablize for 50us or more */
	udelay(100);

	/* release phy from reset */
	if (reset_valid(&reset))
		reset_deassert(&reset);

	/* PHY RS bit should be set after reset */
	switch (phy->id) {
	case USBDPHY1:
	case USBD8PHY1:
	case USBD9PHY1:
		regmap_update_bits(priv->gcr_syscon, GCR_USB1PHYCTL, PHYCTL_RS, PHYCTL_RS);
		break;
	case USBD9PHY2:
	case USBH1PHY2:
		regmap_update_bits(priv->gcr_syscon, GCR_USB2PHYCTL, PHYCTL_RS, PHYCTL_RS);
		break;
	case USBD8PHY3:
	case USBH2PHY3:
		regmap_update_bits(priv->gcr_syscon, GCR_USB3PHYCTL, PHYCTL_RS, PHYCTL_RS);
		break;
	default:
		break;
	}

	return 0;
}

static int npcm_usb_phy_exit(struct phy *phy)
{
	struct npcm_usbphy *priv = dev_get_priv(phy->dev);

	/* set PHY switch to default state */
	switch (phy->id) {
	case USBD8PHY1:
	case USBD8PHY3:
		regmap_update_bits(priv->gcr_syscon, GCR_INTCR3, USBPHY3SW,
				   USBPHY3SW_HOST2);
		break;
	case USBD9PHY1:
	case USBD9PHY2:
		regmap_update_bits(priv->gcr_syscon, GCR_INTCR3, USBPHY2SW,
				   USBPHY2SW_HOST1);
		break;
	default:
		break;
	}
	return 0;
}

static int npcm_usb_phy_probe(struct udevice *dev)
{
	struct npcm_usbphy *priv = dev_get_priv(dev);

	priv->gcr_syscon = syscon_get_regmap(dev->parent);
	if (IS_ERR(priv->gcr_syscon)) {
		dev_err(dev, "%s: unable to get syscon\n", __func__);
		return PTR_ERR(priv->gcr_syscon);
	}

	return 0;
}

static const struct udevice_id npcm_phy_ids[] = {
	{ .compatible = "nuvoton,npcm845-usb-phy",},
	{ }
};

static struct phy_ops npcm_phy_ops = {
	.init = npcm_usb_phy_init,
	.exit = npcm_usb_phy_exit,
};

U_BOOT_DRIVER(npcm_phy) = {
	.name	= "npcm-usb-phy",
	.id	= UCLASS_PHY,
	.of_match = npcm_phy_ids,
	.ops = &npcm_phy_ops,
	.probe		= npcm_usb_phy_probe,
	.priv_auto	= sizeof(struct npcm_usbphy),
};
