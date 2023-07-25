// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2021 JLSemi Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#include "jlsemi-core.h"
#include <linux/phy.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/netdevice.h>

#define JL1XXX_PAGE24		24
#define JL1XXX_LED_BLINK_REG	25

#define JL1XXX_PAGE128		128
#define JL1XXX_LED_GPIO_REG	29
#define JL1XXX_WOL_CTRL_REG	28

#define JL2XXX_PAGE3332		3332
#define JL2XXX_LED_CTRL_REG	16
#define JL2XXX_PAGE4096		4096
#define JL2XXX_LED_BLINK_REG	20
#define JL2XXX_LED_POLARITY_REG	19

#define JL2XXX_PAGE128		128
#define JL2XXX_FLD_CTRL_REG	28
#define JL2XXX_FLD_EN		BIT(13)
#define JL2XXX_FLD_MASK		0x1800
#define JL2XXX_FLD_MASK_HEAD	11
#define JL2XXX_FLD_DELAY_00MS	0
#define JL2XXX_FLD_DELAY_10MS	1
#define JL2XXX_FLD_DELAY_20MS	2
#define JL2XXX_FLD_DELAY_40MS	3

#define JL2XXX_SPEED10		0
#define JL2XXX_SPEED100		1
#define JL2XXX_SPEED1000	2

#define JL2XXX_PHY_MODE_REG	30
#define JL2XXX_FIBER_1000	BIT(12)
#define JL2XXX_FIBER_100	BIT(11)
#define JL2XXX_PHY_FIBER_MODE_MASK	0x1800
#define JL2XXX_BMCR_DUPLEX	BIT(8)
#define JL2XXX_LPA_FIBER_1000HALF	0x40
#define JL2XXX_LPA_FIBER_1000FULL	0x20
#define JL2XXX_BMCR_SPEED_LSB	BIT(13)
#define JL2XXX_BMCR_SPEED_MSB	BIT(6)
#define JL2XXX_BMCR_AN_RESTART	BIT(9)



#define JL2XXX_SUPP_LED_MODE	(JL2XXX_LED0_LINK10 | \
				 JL2XXX_LED0_LINK100 | \
				 JL2XXX_LED0_LINK1000 | \
				 JL2XXX_LED0_ACTIVITY | \
				 JL2XXX_LED1_LINK10 | \
				 JL2XXX_LED1_LINK100 | \
				 JL2XXX_LED1_LINK1000 | \
				 JL2XXX_LED1_ACTIVITY | \
				 JL2XXX_LED2_LINK10 | \
				 JL2XXX_LED2_LINK100 | \
				 JL2XXX_LED2_LINK1000 | \
				 JL2XXX_LED2_ACTIVITY)

#define JL1XXX_SUPP_GPIO	(JL1XXX_GPIO_LED0_EN | \
				 JL1XXX_GPIO_LED0_OUT | \
				 JL1XXX_GPIO_LED1_EN | \
				 JL1XXX_GPIO_LED1_OUT)

#define JL1XXX_SUPP_LED_MODE	(JL1XXX_LED0_EEE | \
				 JL1XXX_LED0_100_ACTIVITY | \
				 JL1XXX_LED0_10_ACTIVITY | \
				 JL1XXX_LED0_100_LINK | \
				 JL1XXX_LED0_10_LINK | \
				 JL1XXX_LED1_EEE | \
				 JL1XXX_LED1_100_ACTIVITY | \
				 JL1XXX_LED1_10_ACTIVITY | \
				 JL1XXX_LED1_100_LINK | \
				 JL1XXX_LED1_10_LINK)

/************************* Configuration section *************************/


/************************* JLSemi iteration code *************************/
static int jl1xxx_led_static_op_set(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	/* Enable LED operation */
	jlsemi_set_bits(phydev, JL1XXX_PAGE7,
			JL1XXX_LED_REG, JL1XXX_LED_EN);

	/* Set led mode */
	if (priv->led.enable & JL1XXX_LED_MODE_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE129,
					      JL1XXX_LED_MODE_REG,
					      JL1XXX_SUPP_LED_MODE,
					      priv->led.mode);
		if (err < 0)
			return err;
	}
	/* Set led period */
	if (priv->led.enable & JL1XXX_LED_GLOABL_PERIOD_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE24,
					      JL1XXX_LED_BLINK_REG,
					      LED_PERIOD_MASK,
					      LEDPERIOD(
					      priv->led.global_period));
		if (err < 0)
			return err;
	}
	/* Set led on time */
	if (priv->led.enable & JL1XXX_LED_GLOBAL_ON_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE24,
					      JL1XXX_LED_BLINK_REG,
					      LED_ON_MASK,
					      LEDON(priv->led.global_on));
		if (err < 0)
			return err;
	}
	/*Set led gpio output */
	if (priv->led.enable & JL1XXX_LED_GPIO_OUT_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE128,
					      JL1XXX_LED_GPIO_REG,
					      JL1XXX_SUPP_GPIO,
					      priv->led.gpio_output);
		if (err < 0)
			return err;
	}

	return 0;
}

static int jl2xxx_led_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	/* Set led mode */
	if (priv->led.enable & JL2XXX_LED_MODE_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE3332,
					      JL2XXX_LED_CTRL_REG,
					      JL2XXX_SUPP_LED_MODE,
					      priv->led.mode);
		if (err < 0)
			return err;
	}
	/* Set led period */
	if (priv->led.enable & JL2XXX_LED_GLOABL_PERIOD_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE4096,
					      JL2XXX_LED_BLINK_REG,
					      LED_PERIOD_MASK,
					      LEDPERIOD(
					      priv->led.global_period));
		if (err < 0)
			return err;
	}
	/* Set led on time */
	if (priv->led.enable & JL2XXX_LED_GLOBAL_ON_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE4096,
					      JL2XXX_LED_BLINK_REG,
					      LED_ON_MASK,
					      LEDON(priv->led.global_on));
		if (err < 0)
			return err;
	}
	/* Set led polarity */
	if (priv->led.enable & JL2XXX_LED_POLARITY_EN) {
		err = jlsemi_set_bits(phydev, JL2XXX_PAGE4096,
				      JL2XXX_LED_POLARITY_REG,
				      priv->led.polarity);
		if (err < 0)
			return err;
	}

	return 0;
}

struct device *jlsemi_get_mdio(struct phy_device *phydev)
{
#if JLSEMI_DEV_COMPATIBLE
	struct device *dev = &phydev->dev;
#else
	struct device *dev = &phydev->mdio.dev;
#endif
	return dev;
}

static struct device_node *get_device_node(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);

	return dev->of_node;
}

static int jl1xxx_dts_led_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl1xxx,led-enable",
			     &priv->led.enable);
	of_property_read_u32(of_node, "jl1xxx,led-mode",
			     &priv->led.mode);
	of_property_read_u32(of_node, "jl1xxx,led-period",
			     &priv->led.global_period);
	of_property_read_u32(of_node, "jl1xxx,led-on",
			     &priv->led.global_on);
	of_property_read_u32(of_node, "jl1xxx,led-gpio",
			     &priv->led.gpio_output);

	return 0;
}

static int jl1xxx_dts_wol_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl1xxx,wol-enable",
			     &priv->wol.enable);

	return 0;
}

static int jl1xxx_dts_intr_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl1xxx,interrupt-enable",
			     &priv->intr.enable);

	return 0;
}

static int jl1xxx_dts_mdi_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl1xxx,mdi-enable",
			     &priv->mdi.enable);
	of_property_read_u32(of_node, "jl1xxx,mdi-rate",
			     &priv->mdi.rate);
	of_property_read_u32(of_node, "jl1xxx,mdi-amplitude",
			     &priv->mdi.amplitude);

	return 0;
}

static int jl1xxx_dts_rmii_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl1xxx,rmii-enable",
			     &priv->rmii.enable);
	of_property_read_u32(of_node, "jl1xxx,rmii-rx_timing",
			     &priv->rmii.rx_timing);
	of_property_read_u32(of_node, "jl1xxx,rmii-tx_timing",
			     &priv->rmii.tx_timing);

	return 0;
}


static int jl2xxx_dts_led_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,led-enable",
			     &priv->led.enable);
	of_property_read_u32(of_node, "jl2xxx,led-mode",
			     &priv->led.mode);
	of_property_read_u32(of_node, "jl2xxx,led-period",
			     &priv->led.global_period);
	of_property_read_u32(of_node, "jl2xxx,led-on",
			     &priv->led.global_on);
	of_property_read_u32(of_node, "jl2xxx,led-polarity",
			     &priv->led.polarity);

	return 0;
}

static int jl1xxx_c_macro_led_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	/* Config LED */
	struct jl_led_ctrl led_cfg = {
		.enable		= JL1XXX_LED_CTRL_EN,
		.mode		= JL1XXX_CFG_LED_MODE,
		.global_period	= JL1XXX_GLOBAL_PERIOD_MS,
		.global_on	= JL1XXX_GLOBAL_ON_MS,
		.gpio_output	= JL1XXX_CFG_GPIO,
	};

	priv->led = led_cfg;

	return 0;
}

static int jl1xxx_c_macro_wol_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	struct jl_wol_ctrl wol_cfg = {
		.enable		= JL1XXX_WOL_CTRL_EN,
	};

	priv->wol = wol_cfg;

	return 0;
}

static int jl1xxx_c_macro_intr_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	struct jl_intr_ctrl intr_cfg = {
		.enable		= JL1XXX_INTR_CTRL_EN,
	};

	priv->intr = intr_cfg;

	return 0;
}

static int jl1xxx_c_macro_mdi_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	struct jl_mdi_ctrl mdi_cfg = {
		.enable		= JL1XXX_MDI_CTRL_EN,
		.rate		= JL1XXX_MDI_RATE,
		.amplitude	= JL1XXX_MDI_AMPLITUDE,
	};

	priv->mdi = mdi_cfg;

	return 0;
}

static int jl1xxx_c_macro_rmii_cfg_get(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;

	struct jl_rmii_ctrl rmii_cfg = {
		.enable		= JL1XXX_RMII_CTRL_EN,
		.tx_timing	= JL1XXX_RMII_TX_TIMING,
		.rx_timing	= JL1XXX_RMII_RX_TIMING,
	};

	priv->rmii = rmii_cfg;

	return 0;
}

static int jl2xxx_c_macro_led_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_led_ctrl led_cfg = {
		.enable		= JL2XXX_LED_CTRL_EN,
		.mode		= JL2XXX_CFG_LED_MODE,
		.global_period	= JL2XXX_GLOBAL_PERIOD_MS,
		.global_on	= JL2XXX_GLOBAL_ON_MS,
		.polarity	= JL2XXX_LED_POLARITY,
	};

	priv->led = led_cfg;

	return 0;
}

static int jl1xxx_wol_operation_args(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_wol_ctrl *wol = &priv->wol;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl1xxx_dts_wol_cfg_get(phydev);
	else
		jl1xxx_c_macro_wol_cfg_get(phydev);

	/* Supported by default */
	wol->ethtool = false;

	return 0;
}

static int jl1xxx_intr_operation_args(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_intr_ctrl *intr = &priv->intr;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl1xxx_dts_intr_cfg_get(phydev);
	else
		jl1xxx_c_macro_intr_cfg_get(phydev);

	/* Not supported by default */
	intr->ethtool = false;

	return 0;
}

static int jl1xxx_mdi_operation_args(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_mdi_ctrl *mdi = &priv->mdi;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl1xxx_dts_mdi_cfg_get(phydev);
	else
		jl1xxx_c_macro_mdi_cfg_get(phydev);

	/* Not supported by default */
	mdi->ethtool = false;

	return 0;
}

static int jl1xxx_rmii_operation_args(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_rmii_ctrl *rmii = &priv->rmii;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl1xxx_dts_rmii_cfg_get(phydev);
	else
		jl1xxx_c_macro_rmii_cfg_get(phydev);

	/* Not supported by default */
	rmii->ethtool = false;

	return 0;
}

static int jl1xxx_led_operation_args(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	struct jl_led_ctrl *led = &priv->led;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl1xxx_dts_led_cfg_get(phydev);
	else
		jl1xxx_c_macro_led_cfg_get(phydev);

	/* Not supported by default */
	led->ethtool = false;

	return 0;
}

static int jl2xxx_led_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_led_ctrl *led = &priv->led;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_led_cfg_get(phydev);
	else
		jl2xxx_c_macro_led_cfg_get(phydev);

	/* Not supported by default */
	led->ethtool = false;

	return 0;
}


static int jl2xxx_dts_fld_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,fld-enable",
			     &priv->fld.enable);
	of_property_read_u32(of_node, "jl2xxx,fld-delay",
			     &priv->fld.delay);

	return 0;
}

static int jl2xxx_dts_wol_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,wol-enable",
			     &priv->wol.enable);

	return 0;
}

static int jl2xxx_dts_intr_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,interrupt-enable",
			     &priv->intr.enable);

	return 0;
}

static int jl2xxx_dts_downshift_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,downshift-enable",
			     &priv->downshift.enable);
	of_property_read_u32(of_node, "jl2xxx,downshift-count",
			     &priv->downshift.count);

	return 0;
}

static int jl2xxx_dts_rgmii_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,rgmii-enable",
			     &priv->rgmii.enable);
	of_property_read_u32(of_node, "jl2xxx,rgmii-tx-delay",
			     &priv->rgmii.tx_delay);
	of_property_read_u32(of_node, "jl2xxx,rgmii-rx-delay",
			     &priv->rgmii.rx_delay);

	return 0;
}

static int jl2xxx_dts_patch_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,patch-enable",
			     &priv->patch.enable);

	return 0;
}

static int jl2xxx_dts_clk_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,clk-enable",
			     &priv->clk.enable);

	return 0;
}

static int jl2xxx_dts_work_mode_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,work_mode-enable",
			     &priv->work_mode.enable);
	of_property_read_u32(of_node, "jl2xxx,work_mode-mode",
			     &priv->work_mode.mode);

	return 0;
}

static int jl2xxx_dts_lpbk_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,lpbk-enable",
			     &priv->lpbk.enable);
	of_property_read_u32(of_node, "jl2xxx,lpbk-mode",
			     &priv->lpbk.mode);

	return 0;
}

static int jl2xxx_dts_slew_rate_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,slew_rate-enable",
			     &priv->slew_rate.enable);

	return 0;
}

static int jl2xxx_dts_rxc_out_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct device_node *of_node = get_device_node(phydev);

	of_property_read_u32(of_node, "jl2xxx,rxc_out-enable",
			     &priv->rxc_out.enable);

	return 0;
}

static int jl2xxx_c_macro_fld_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_fld_ctrl fld_cfg = {
		.enable		= JL2XXX_FLD_CTRL_EN,
		.delay		= JL2XXX_FLD_DELAY,
	};

	priv->fld = fld_cfg;

	return 0;
}

static int jl2xxx_c_macro_wol_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_wol_ctrl wol_cfg = {
		.enable		= JL2XXX_WOL_CTRL_EN,
	};

	priv->wol = wol_cfg;

	return 0;
}

static int jl2xxx_c_macro_intr_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_intr_ctrl intr_cfg = {
		.enable		= JL2XXX_INTR_CTRL_EN,
	};

	priv->intr = intr_cfg;

	return 0;
}

static int jl2xxx_c_macro_downshift_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_downshift_ctrl downshift_cfg = {
		.enable		= JL2XXX_DSFT_CTRL_EN,
		.count		= JL2XXX_DSFT_AN_CNT,
	};

	priv->downshift = downshift_cfg;

	return 0;
}

static int jl2xxx_c_macro_rgmii_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_rgmii_ctrl rgmii_cfg = {
		.enable		= JL2XXX_RGMII_CTRL_EN,
		.rx_delay	= JL2XXX_RGMII_RX_DLY_2NS,
		.tx_delay	= JL2XXX_RGMII_TX_DLY_2NS,
	};

	priv->rgmii = rgmii_cfg;

	return 0;
}

static int jl2xxx_c_macro_patch_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_patch_ctrl patch_cfg = {
		.enable		= JL2XXX_PATCH_CTRL_EN,
	};

	priv->patch = patch_cfg;

	return 0;
}

static int jl2xxx_c_macro_clk_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_clk_ctrl clk_cfg = {
		.enable		= JL2XXX_CLK_CTRL_EN,
	};

	priv->clk = clk_cfg;

	return 0;
}

static int jl2xxx_c_macro_work_mode_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_work_mode_ctrl work_mode_cfg = {
		.enable		= JL2XXX_WORK_MODE_CTRL_EN,
		.mode		= JL2XXX_WORK_MODE_MODE,
	};

	priv->work_mode = work_mode_cfg;

	return 0;
}

static int jl2xxx_c_macro_lpbk_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_loopback_ctrl lpbk_cfg = {
		.enable		= JL2XXX_LPBK_CTRL_EN,
		.mode		= JL2XXX_LPBK_MODE,
	};

	priv->lpbk = lpbk_cfg;

	return 0;
}

static int jl2xxx_c_macro_slew_rate_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_slew_rate_ctrl slew_rate_cfg = {
		.enable		= JL2XXX_SLEW_RATE_CTRL_EN,
	};

	priv->slew_rate = slew_rate_cfg;

	return 0;
}

static int jl2xxx_c_macro_rxc_out_cfg_get(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	struct jl_rxc_out_ctrl rxc_out_cfg = {
		.enable		= JL2XXX_RXC_OUT_CTRL_EN,
	};

	priv->rxc_out = rxc_out_cfg;

	return 0;
}

static int jl2xxx_fld_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_fld_ctrl *fld = &priv->fld;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_fld_cfg_get(phydev);
	else
		jl2xxx_c_macro_fld_cfg_get(phydev);

	/* Supported by default */
	fld->ethtool = false;

	return 0;
}

static int jl2xxx_wol_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_wol_ctrl *wol = &priv->wol;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_wol_cfg_get(phydev);
	else
		jl2xxx_c_macro_wol_cfg_get(phydev);

	/* Supported by default */
	wol->ethtool = false;

	return 0;
}

static int jl2xxx_intr_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_intr_ctrl *intr = &priv->intr;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_intr_cfg_get(phydev);
	else
		jl2xxx_c_macro_intr_cfg_get(phydev);

	/* Not supported by default */
	intr->ethtool = false;

	return 0;
}

static int jl2xxx_downshift_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_downshift_ctrl *downshift = &priv->downshift;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_downshift_cfg_get(phydev);
	else
		jl2xxx_c_macro_downshift_cfg_get(phydev);

	/* Supported by default */
	downshift->ethtool = false;

	return 0;
}

static int jl2xxx_rgmii_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_rgmii_ctrl *rgmii = &priv->rgmii;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_rgmii_cfg_get(phydev);
	else
		jl2xxx_c_macro_rgmii_cfg_get(phydev);

	/* Not supported by default */
	rgmii->ethtool = false;

	return 0;
}

static int jl2xxx_patch_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_patch_ctrl *patch = &priv->patch;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_patch_cfg_get(phydev);
	else
		jl2xxx_c_macro_patch_cfg_get(phydev);

	/* Not supported by default */
	patch->ethtool = false;

	return 0;
}

static int jl2xxx_clk_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_clk_ctrl *clk = &priv->clk;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_clk_cfg_get(phydev);
	else
		jl2xxx_c_macro_clk_cfg_get(phydev);

	/* Not supported by default */
	clk->ethtool = false;

	return 0;
}

static int jl2xxx_work_mode_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_work_mode_ctrl *work_mode = &priv->work_mode;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_work_mode_cfg_get(phydev);
	else
		jl2xxx_c_macro_work_mode_cfg_get(phydev);

	/* Not supported by default */
	work_mode->ethtool = false;

	return 0;
}

static int jl2xxx_lpbk_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_loopback_ctrl *lpbk = &priv->lpbk;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_lpbk_cfg_get(phydev);
	else
		jl2xxx_c_macro_lpbk_cfg_get(phydev);

	/* Not supported by default */
	lpbk->ethtool = false;

	return 0;
}

static int jl2xxx_slew_rate_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_slew_rate_ctrl *slew_rate = &priv->slew_rate;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_slew_rate_cfg_get(phydev);
	else
		jl2xxx_c_macro_slew_rate_cfg_get(phydev);

	/* Not supported by default */
	slew_rate->ethtool = false;

	return 0;
}

static int jl2xxx_rxc_out_operation_args(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	struct jl_rxc_out_ctrl *rxc_out = &priv->rxc_out;

	if (JLSEMI_KERNEL_DEVICE_TREE_USE)
		jl2xxx_dts_rxc_out_cfg_get(phydev);
	else
		jl2xxx_c_macro_rxc_out_cfg_get(phydev);

	/* Not supported by default */
	rxc_out->ethtool = false;

	return 0;
}

static int jl2xxx_fld_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;
	u8 val;

	val = priv->fld.delay & 0xff;
	err = jl2xxx_fld_dynamic_op_set(phydev, &val);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_wol_cfg_rmii(struct phy_device *phydev)
{
	int err;
	/* WOL Function should be in RMII Mode, the rmii
	 * clock direction should be output
	 */
	err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE7,
				      JL1XXX_RMII_CTRL_REG,
				      JL1XXX_RMII_OUT,
				      JL1XXX_RMII_MODE);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_wol_clear(struct phy_device *phydev)
{
	jlsemi_set_bits(phydev, JL1XXX_PAGE129,
			JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_CLEAR);

	jlsemi_clear_bits(phydev, JL1XXX_PAGE129,
			  JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_CLEAR);

	return 0;
}

static bool jl1xxx_wol_receive_check(struct phy_device *phydev)
{
	if (jlsemi_fetch_bit(phydev, JL1XXX_PAGE129,
			     JL1XXX_WOL_CTRL_REG, JL1xxx_WOL_RECEIVE))
		return true;
	else
		return false;

}

static int jl1xxx_wol_enable(struct phy_device *phydev, bool enable)
{
	if (enable)
		jlsemi_clear_bits(phydev, JL1XXX_PAGE129,
				  JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_DIS);
	else
		jlsemi_set_bits(phydev, JL1XXX_PAGE129,
				JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_DIS);

	return 0;
}
static int jl1xxx_wol_store_mac_addr(struct phy_device *phydev)
{
	int err;

	jlsemi_write_page(phydev, JL1XXX_PAGE129);

	/* Store the device address for the magic packet */
	err = phy_write(phydev, JL1XXX_MAC_ADDR2_REG,
			((ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[0]) << 8) |
			  ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[1])));
	if (err < 0)
		return err;
	err = phy_write(phydev, JL1XXX_MAC_ADDR1_REG,
			((ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[2]) << 8) |
			  ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[3])));
	if (err < 0)
		return err;
	err = phy_write(phydev, JL1XXX_MAC_ADDR0_REG,
			((ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[4]) << 8) |
			  ADDR8_HIGH_TO_LOW(
			  phydev->attached_dev->dev_addr[5])));
	if (err < 0)
		return err;

	/* change page to 0 */
	jlsemi_write_page(phydev, JL1XXX_PAGE0);

	return 0;
}

static int jl2xxx_wol_enable(struct phy_device *phydev, bool enable)
{
	if (enable) {
		jlsemi_set_bits(phydev, JL2XXX_WOL_CTRL_PAGE,
				JL2XXX_WOL_CTRL_REG, JL2XXX_WOL_GLB_EN);
		jlsemi_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EN);
	} else {
		jlsemi_clear_bits(phydev, JL2XXX_WOL_CTRL_PAGE,
				  JL2XXX_WOL_CTRL_REG, JL2XXX_WOL_GLB_EN);
		jlsemi_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EN);
	}
	jlsemi_soft_reset(phydev);

	return 0;
}

static int jl2xxx_wol_active_low_polarity(struct phy_device *phydev, bool low)
{
	if (low)
		jlsemi_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				JL2XXX_WOL_STAS_REG, JL2XXX_WOL_POLARITY);
	else
		jlsemi_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
				  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_POLARITY);

	return 0;
}

static int jl2xxx_wol_clear(struct phy_device *phydev)
{
	jlsemi_set_bits(phydev, JL2XXX_WOL_STAS_PAGE,
			JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EVENT);
	jlsemi_clear_bits(phydev, JL2XXX_WOL_STAS_PAGE,
			  JL2XXX_WOL_STAS_REG, JL2XXX_WOL_EVENT);

	return 0;
}

static int jl2xxx_store_mac_addr(struct phy_device *phydev)
{
	int err;

	jlsemi_write_page(phydev, JL2XXX_WOL_STAS_PAGE);

	/* Store the device address for the magic packet */
	err = phy_write(phydev, JL2XXX_MAC_ADDR2_REG,
			((phydev->attached_dev->dev_addr[0] << 8) |
			  phydev->attached_dev->dev_addr[1]));
	if (err < 0)
		return err;
	err = phy_write(phydev, JL2XXX_MAC_ADDR1_REG,
			((phydev->attached_dev->dev_addr[2] << 8) |
			  phydev->attached_dev->dev_addr[3]));
	if (err < 0)
		return err;
	err = phy_write(phydev, JL2XXX_MAC_ADDR0_REG,
			((phydev->attached_dev->dev_addr[4] << 8) |
			  phydev->attached_dev->dev_addr[5]));
	if (err < 0)
		return err;

	/* change page to 0 */
	jlsemi_write_page(phydev, JL2XXX_PAGE0);

	return 0;
}

/* Get fast link down for jl2xxx */
#if (JL2XXX_PHY_TUNABLE)
int jl2xxx_fld_dynamic_op_get(struct phy_device *phydev, u8 *msecs)
{
	int ret;
	u16 val;

	ret = jlsemi_read_paged(phydev, JL2XXX_PAGE128,
				JL2XXX_FLD_CTRL_REG);

	if (ret < 0)
		return ret;

	if (ret & JL2XXX_FLD_EN) {
		*msecs = ETHTOOL_PHY_FAST_LINK_DOWN_OFF;
		return 0;
	}

	val = (ret & JL2XXX_FLD_MASK) >> JL2XXX_FLD_MASK_HEAD;

	switch (val) {
	case JL2XXX_FLD_DELAY_00MS:
		*msecs = 0;
		break;
	case JL2XXX_FLD_DELAY_10MS:
		*msecs = 10;
		break;
	case JL2XXX_FLD_DELAY_20MS:
		*msecs = 20;
		break;
	case JL2XXX_FLD_DELAY_40MS:
		*msecs = 40;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
#endif
/* Set fast link down for jl2xxx */
int jl2xxx_fld_dynamic_op_set(struct phy_device *phydev, const u8 *msecs)
{
	u16 val;
	int ret;

#if (JL2XXX_PHY_TUNABLE)
	if (*msecs == ETHTOOL_PHY_FAST_LINK_DOWN_OFF)
		return jlsemi_clear_bits(phydev, JL2XXX_PAGE128,
					 JL2XXX_FLD_CTRL_REG,
					 JL2XXX_FLD_EN);
#endif
	if (*msecs <= 5)
		val = JL2XXX_FLD_DELAY_00MS;
	else if (*msecs <= 15)
		val = JL2XXX_FLD_DELAY_10MS;
	else if (*msecs <= 30)
		val = JL2XXX_FLD_DELAY_20MS;
	else
		val = JL2XXX_FLD_DELAY_40MS;

	val = val << JL2XXX_FLD_MASK_HEAD;

	ret = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE128,
				      JL2XXX_FLD_CTRL_REG,
				      JL2XXX_FLD_MASK, val);
	if (ret < 0)
		return ret;

	ret = jlsemi_set_bits(phydev, JL2XXX_PAGE128,
			      JL2XXX_FLD_CTRL_REG,
			      JL2XXX_FLD_EN);
	if (ret < 0)
		return ret;

	return 0;
}

int jl2xxx_downshift_dynamic_op_get(struct phy_device *phydev, u8 *data)
{
	int val, cnt, enable;

	val = jlsemi_read_paged(phydev, JL2XXX_PAGE0,
				JL2XXX_DSFT_CTRL_REG);

	if (val < 0)
		return val;

	enable = val & JL2XXX_DSFT_EN;
	cnt = (val & JL2XXX_DSFT_AN_MASK) + 1;

#if (JL2XXX_PHY_TUNABLE)
	*data = enable ? cnt : DOWNSHIFT_DEV_DISABLE;
#else
	*data = enable ? cnt : 0;
#endif
	return 0;
}

int jl2xxx_downshift_dynamic_op_set(struct phy_device *phydev, u8 cnt)
{
	int val, err;

	if (cnt > JL2XXX_DSFT_CNT_MAX)
		return -E2BIG;

	if (!cnt) {
		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE0,
					JL2XXX_DSFT_CTRL_REG,
					JL2XXX_DSFT_EN);
	} else {
		val = ((cnt - 1) & JL2XXX_DSFT_AN_MASK) | JL2XXX_DSFT_EN |
			JL2XXX_DSFT_SMART_EN | JL2XXX_DSFT_AN_ERR_EN |
			JL2XXX_DSFT_STL_CNT(18);
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE0,
					      JL2XXX_DSFT_CTRL_REG,
					      JL2XXX_DSFT_AN_MASK, val);
	}

	if (err < 0)
		return err;

	return 0;
}

int jl2xxx_downshift_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	err = jl2xxx_downshift_dynamic_op_set(phydev,
					      priv->downshift.count);
	if (err < 0)
		return err;

	return 0;
}

int jl2xxx_rgmii_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->rgmii.enable & JL2XXX_RGMII_TX_DLY_EN) {
		err = jlsemi_set_bits(phydev, JL2XXX_PAGE3336,
				      JL2XXX_RGMII_CTRL_REG,
				      priv->rgmii.tx_delay);
		if (err < 0)
			return err;

	} else {
		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE3336,
					JL2XXX_RGMII_CTRL_REG,
					priv->rgmii.tx_delay);
		if (err < 0)
			return err;
	}

	if (priv->rgmii.enable & JL2XXX_RGMII_RX_DLY_EN) {
		err = jlsemi_set_bits(phydev, JL2XXX_PAGE3336,
				      JL2XXX_RGMII_CTRL_REG,
				      priv->rgmii.rx_delay);
		if (err < 0)
			return err;
	} else {
		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE3336,
				      JL2XXX_RGMII_CTRL_REG,
				      priv->rgmii.rx_delay);
		if (err < 0)
			return err;
	}

	err = jlsemi_soft_reset(phydev);
	if (err < 0)
		return err;

	return 0;
}

int jl2xxx_clk_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->clk.enable & JL2XXX_125M_CLK_OUT_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE2627,
					      JL2XXX_CLK_CTRL_REG,
					      JL2XXX_CLK_SSC_EN,
					      JL2XXX_CLK_OUT_PIN |
					      JL2XXX_CLK_125M_OUT |
					      JL2XXXX_CLK_SRC);
		if (err < 0)
			return err;
	} else if (priv->clk.enable & JL2XXX_25M_CLK_OUT_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE2627,
					      JL2XXX_CLK_CTRL_REG,
					      JL2XXX_CLK_SSC_EN |
					      JL2XXX_CLK_125M_OUT,
					      JL2XXX_CLK_OUT_PIN |
					      JL2XXXX_CLK_SRC);
		if (err < 0)
			return err;
	} else if (priv->clk.enable & JL2XXX_CLK_OUT_DIS) {
		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE2627,
					JL2XXX_CLK_CTRL_REG,
					JL2XXX_CLK_OUT_PIN);
		if (err < 0)
			return err;
	}

	err = jlsemi_soft_reset(phydev);
	if (err < 0)
		return err;

	return 0;
}

int jl2xxx_slew_rate_static_op_set(struct phy_device *phydev)
{
	int err;

	err = jlsemi_set_bits(phydev, JL2XXX_PAGE258,
			      JL2XXX_SLEW_RATE_CTRL_REG,
			      JL2XXX_SLEW_RATE_EN | JL2XXX_SLEW_RATE_REF_CLK |
			      JL2XXX_SLEW_RATE_SEL_CLK);
	if (err < 0)
		return err;

	return 0;
}

int jl2xxx_rxc_out_static_op_set(struct phy_device *phydev)
{
	int err;

	err = jlsemi_set_bits(phydev, JL2XXX_PAGE18,
			      JL2XXX_RXC_OUT_REG, JL2XXX_RXC_OUT);
	if (err < 0)
		return err;

	err = jlsemi_soft_reset(phydev);
	if (err < 0)
		return err;

	return 0;
}

int jl2xxx_work_mode_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE18,
				JL2XXX_WORK_MODE_REG,
				JL2XXX_WORK_MODE_MASK,
				priv->work_mode.mode);

	err = jlsemi_soft_reset(phydev);
	if (err < 0)
		return err;

	return 0;
}

static inline int __genphy_setup_forced(struct phy_device *phydev)
{
	int err;
	int ctl = 0;

	phydev->pause = phydev->asym_pause = 0;

	if (phydev->speed == SPEED_1000)
		ctl |= BMCR_SPEED1000;
	else if (phydev->speed == SPEED_100)
		ctl |= BMCR_SPEED100;

	if (phydev->duplex == DUPLEX_FULL)
		ctl |= BMCR_FULLDPLX;

	err = phy_write(phydev, MII_BMCR, ctl);

	return err;
}

int jl2xxx_config_aneg_fiber(struct phy_device *phydev)
{
	if (phydev->autoneg != AUTONEG_ENABLE)
		return __genphy_setup_forced(phydev);

	/* Dou to fiber auto mode only support 1000M,
	 * we set 1000M speed to reg0
	 * NOTE: Do need restart AN otherwise will link down
	 */
	jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE0,
				JL2XXX_BMCR_REG,
				JL2XXX_BMCR_SPEED_LSB,
				JL2XXX_BMCR_SPEED_MSB | BMCR_ANENABLE);
	return 0;
}

static int jl2xxx_fiber_autoneg_config(struct phy_device *phydev)
{
	int speed;
	int duplex;

	speed = jlsemi_read_paged(phydev, JL2XXX_PAGE0, JL2XXX_PHY_MODE_REG);
	if (speed < 0)
		return speed;

	duplex = jlsemi_fetch_bit(phydev, JL2XXX_PAGE0,
				  MII_BMCR, JL2XXX_BMCR_DUPLEX);
	if (duplex < 0)
		return duplex;

	if (duplex)
		phydev->duplex = DUPLEX_FULL;
	else
		phydev->duplex = DUPLEX_HALF;

	speed &= JL2XXX_PHY_FIBER_MODE_MASK;
	switch (speed) {
	case JL2XXX_FIBER_1000:
		phydev->speed = SPEED_1000;
		break;
	case JL2XXX_FIBER_100:
		phydev->speed = SPEED_100;
		break;
	default:
		break;
	}

	return 0;
}

static int jl2xxx_update_fiber_status(struct phy_device *phydev)
{
	int status;
	int link;

	status = jlsemi_read_paged(phydev, JL2XXX_PAGE0, JL2XXX_PHY_MODE_REG);
	if (status < 0)
		return status;

	link = status & JL2XXX_PHY_FIBER_MODE_MASK;

	if (link)
		phydev->link = 1;
	else
		phydev->link = 0;

	if (phydev->autoneg == AUTONEG_ENABLE)
		jl2xxx_fiber_autoneg_config(phydev);

	return 0;
}

bool jl2xxx_read_fiber_status(struct phy_device *phydev)
{
	bool fiber_ok = false;
	u16 phy_mode;
	int val;

	val = jlsemi_read_paged(phydev, JL2XXX_PAGE18, JL2XXX_WORK_MODE_REG);
	phy_mode = val & JL2XXX_WORK_MODE_MASK;

	if ((phydev->interface != PHY_INTERFACE_MODE_SGMII) &&
	   ((phy_mode == JL2XXX_FIBER_RGMII_MODE) ||
	    (phy_mode == JL2XXX_UTP_FIBER_RGMII_MODE))) {
		jl2xxx_update_fiber_status(phydev);
		if (phydev->link)
			fiber_ok = true;
	}

	return fiber_ok;
}

static int jl2xxx_force_speed(struct phy_device *phydev, u16 speed)
{
	int err;

	if (speed == JL2XXX_SPEED1000)
		jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE0, MII_BMCR,
					BMCR_SPEED100, BMCR_SPEED1000);
	else if (speed == JL2XXX_SPEED100)
		jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE0, MII_BMCR,
					BMCR_SPEED1000, BMCR_SPEED100);
	else if (speed == JL2XXX_SPEED10)
		jlsemi_clear_bits(phydev, JL2XXX_PAGE0, MII_BMCR,
				  BMCR_SPEED1000 | BMCR_SPEED100);

	err = jlsemi_clear_bits(phydev, JL2XXX_PAGE0, MII_BMCR,
				BMCR_ANENABLE);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_lpbk_force_speed(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;

	if (priv->lpbk.mode == JL2XXX_LPBK_PCS_1000M)
		jl2xxx_force_speed(phydev, JL2XXX_SPEED1000);
	else if (priv->lpbk.mode == JL2XXX_LPBK_PCS_100M)
		jl2xxx_force_speed(phydev, JL2XXX_SPEED100);
	else if (priv->lpbk.mode == JL2XXX_LPBK_PCS_10M)
		jl2xxx_force_speed(phydev, JL2XXX_SPEED10);

	return 0;
}

int jl2xxx_lpbk_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if ((priv->lpbk.mode == JL2XXX_LPBK_PCS_10M) ||
	    (priv->lpbk.mode == JL2XXX_LPBK_PCS_100M) ||
	    (priv->lpbk.mode == JL2XXX_LPBK_PCS_1000M)) {
		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE0,
					      MII_BMCR, BMCR_LOOPBACK,
					      BMCR_LOOPBACK);
		if (err < 0)
			return err;
		err = jl2xxx_lpbk_force_speed(phydev);
		if (err < 0)
			return err;
	} else if (priv->lpbk.mode == JL2XXX_LPBK_PMD_1000M) {
		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE160,
					JL2XXX_REG25, JL2XXX_CPU_RESET);
		if (err < 0)
			return err;

		jlsemi_write_page(phydev, JL2XXX_PAGE173);
		phy_write(phydev, JL2XXX_REG16, JL2XXX_LOAD_GO);
		phy_write(phydev, JL2XXX_REG17, JL2XXX_LOAD_DATA0);
		jlsemi_write_page(phydev, JL2XXX_PAGE0);

		err = jlsemi_set_bits(phydev, JL2XXX_PAGE160,
				      JL2XXX_REG25, JL2XXX_CPU_RESET);
		if (err < 0)
			return err;

		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE0,
					      JL2XXX_REG20,
					      JL2XXX_LPBK_MODE_MASK,
					      JL2XXX_LPBK_PMD_MODE);
		if (err < 0)
			return err;

		err = jlsemi_set_bits(phydev, JL2XXX_PAGE18, JL2XXX_REG21,
				      JL2XXX_SPEED1000_NO_AN);
		if (err < 0)
			return err;

		err = jlsemi_soft_reset(phydev);
		if (err < 0)
			return err;

	} else if (priv->lpbk.mode == JL2XXX_LPBK_EXT_STUB_1000M) {
		err = jlsemi_clear_bits(phydev, JL2XXX_PAGE160,
					JL2XXX_REG25, JL2XXX_CPU_RESET);
		if (err < 0)
			return err;
		jlsemi_write_page(phydev, JL2XXX_PAGE173);
		phy_write(phydev, JL2XXX_REG16, JL2XXX_LOAD_GO);
		phy_write(phydev, JL2XXX_REG17, JL2XXX_LOAD_DATA0);
		jlsemi_write_page(phydev, JL2XXX_PAGE0);

		err = jlsemi_set_bits(phydev, JL2XXX_PAGE160,
				      JL2XXX_REG25, JL2XXX_CPU_RESET);
		if (err < 0)
			return err;

		err = jlsemi_modify_paged_reg(phydev, JL2XXX_PAGE0,
					      JL2XXX_REG20,
					      JL2XXX_LPBK_MODE_MASK,
					      JL2XXX_LPBK_EXT_MODE);
		if (err < 0)
			return err;

		err = jlsemi_set_bits(phydev, JL2XXX_PAGE18, JL2XXX_REG21,
				      JL2XXX_SPEED1000_NO_AN);
		if (err < 0)
			return err;

		err = jlsemi_soft_reset(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl1xxx_mdi_static_op_set(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->mdi.enable & JL1XXX_MDI_RATE_EN) {
		err = jlsemi_set_bits(phydev, JL1XXX_PAGE24,
				      JL1XXX_REG24, priv->mdi.rate);
		if (err < 0)
			return err;
	}

	if (priv->mdi.enable & JL1XXX_MDI_AMPLITUDE_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE24,
					      JL1XXX_REG24,
					      JL1XXX_MDI_TX_BM_MASK,
					      JL1XXX_MDI_TX_BM(
					      priv->mdi.amplitude));
		if (err < 0)
			return err;
	}

	return 0;
}

int jl1xxx_rmii_static_op_set(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->rmii.enable & JL1XXX_RMII_MODE_EN) {
		err = jlsemi_set_bits(phydev, JL1XXX_PAGE7,
				      JL1XXX_REG16, JL1XXX_RMII_MODE);
		if (err < 0)
			return err;
	} else {
		err = jlsemi_clear_bits(phydev, JL1XXX_PAGE7,
					JL1XXX_REG16, JL1XXX_RMII_MODE);
		if (err < 0)
			return err;
		return 0;
	}

	if (priv->rmii.enable & JL1XXX_RMII_CLK_50M_INPUT_EN) {
		err = jlsemi_set_bits(phydev, JL1XXX_PAGE7,
				      JL1XXX_REG16,
				      JL1XXX_RMII_CLK_50M_INPUT);
		if (err < 0)
			return err;
	} else {
		err = jlsemi_clear_bits(phydev, JL1XXX_PAGE7,
					JL1XXX_REG16,
					JL1XXX_RMII_CLK_50M_INPUT);
		if (err < 0)
			return err;
	}

	if (priv->rmii.enable & JL1XXX_RMII_CRS_DV_EN) {
		err = jlsemi_set_bits(phydev, JL1XXX_PAGE7,
				      JL1XXX_REG16,
				      JL1XXX_RMII_CRS_DV);
		if (err < 0)
			return err;
	} else {
		err = jlsemi_clear_bits(phydev, JL1XXX_PAGE7,
					JL1XXX_REG16,
					JL1XXX_RMII_CRS_DV);
		if (err < 0)
			return err;
	}

	if (priv->rmii.enable & JL1XXX_RMII_TX_SKEW_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE7,
					      JL1XXX_REG16,
					      JL1XXX_RMII_TX_SKEW_MASK,
					      JL1XXX_RMII_TX_SKEW(
					      priv->rmii.tx_timing));
		if (err < 0)
			return err;
	}

	if (priv->rmii.enable & JL1XXX_RMII_RX_SKEW_EN) {
		err = jlsemi_modify_paged_reg(phydev, JL1XXX_PAGE7,
					      JL1XXX_REG16,
					      JL1XXX_RMII_RX_SKEW_MASK,
					      JL1XXX_RMII_RX_SKEW(
					      priv->rmii.rx_timing));
		if (err < 0)
			return err;
	}

	return 0;
}

static u16 patch_fw_versions0[] = {0x9101, 0x9107};
static u16 patch_fw_versions1[] = {0x1101};
static u16 patch_fw_versions2[] = {0x930a};

static u16 patch_version0 = 0xdef2;
static u32 init_data0[] = {
	0x1f00a0, 0x1903f3, 0x1f0012, 0x150100, 0x1f00ad, 0x100000,
	0x11e0c6, 0x1f00a0, 0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb,
	0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb, 0x1f00ad, 0x110000,
	0x120400, 0x130093, 0x140000, 0x150193, 0x160000, 0x170213,
	0x180000, 0x12040c, 0x130293, 0x140000, 0x150313, 0x160000,
	0x170393, 0x180000, 0x120418, 0x130413, 0x140000, 0x150493,
	0x160000, 0x170513, 0x180000, 0x120424, 0x130593, 0x140000,
	0x150613, 0x160000, 0x170693, 0x180000, 0x120430, 0x130713,
	0x140000, 0x150793, 0x160000, 0x171137, 0x180000, 0x12043c,
	0x13006f, 0x140060, 0x15a001, 0x160113, 0x17fd41, 0x18d026,
	0x120448, 0x13d406, 0x14d222, 0x1517b7, 0x160800, 0x17aa23,
	0x189407, 0x120454, 0x130713, 0x1430f0, 0x1567b7, 0x160800,
	0x17a423, 0x1846e7, 0x120460, 0x13a703, 0x14a587, 0x156685,
	0x168f55, 0x17ac23, 0x18a4e7, 0x12046c, 0x1367b9, 0x145737,
	0x150800, 0x168793, 0x17ef27, 0x182023, 0x120478, 0x1374f7,
	0x1407b7, 0x150800, 0x165bfc, 0x17d493, 0x180037, 0x120484,
	0x13f493, 0x141f04, 0x15f793, 0x1607f7, 0x178fc5, 0x18c03e,
	0x120490, 0x134702, 0x140793, 0x150210, 0x160763, 0x1700f7,
	0x180793, 0x12049c, 0x130270, 0x140c63, 0x1530f7, 0x16a001,
	0x1707b7, 0x180002, 0x1204a8, 0x138793, 0x146967, 0x15c83e,
	0x1617b7, 0x170002, 0x188793, 0x1204b4, 0x13e567, 0x14c43e,
	0x1537b7, 0x160002, 0x178793, 0x186867, 0x1204c0, 0x13c23e,
	0x1447b7, 0x150002, 0x168793, 0x17e9a7, 0x1866b7, 0x1204cc,
	0x130800, 0x14ca3e, 0x15a783, 0x166d86, 0x1775c1, 0x188713,
	0x1204d8, 0x130ff5, 0x148ff9, 0x156735, 0x160713, 0x178007,
	0x188fd9, 0x1204e4, 0x13ac23, 0x146cf6, 0x15a783, 0x1665c6,
	0x175737, 0x180800, 0x1204f0, 0x136611, 0x14f793, 0x15f0f7,
	0x16e793, 0x170807, 0x18ae23, 0x1204fc, 0x1364f6, 0x142783,
	0x155c47, 0x169bf5, 0x172223, 0x185cf7, 0x120508, 0x13a703,
	0x14f5c6, 0x158f51, 0x16ae23, 0x17f4e6, 0x180737, 0x120514,
	0x130809, 0x14433c, 0x158fd1, 0x16c33c, 0x170637, 0x180800,
	0x120520, 0x134a74, 0x14679d, 0x158793, 0x160e07, 0x179ae1,
	0x18e693, 0x12052c, 0x130036, 0x14ca74, 0x154678, 0x1676e1,
	0x178693, 0x185006, 0x120538, 0x138ff9, 0x148fd5, 0x1507c2,
	0x168f6d, 0x1783c1, 0x188fd9, 0x120544, 0x13c67c, 0x140713,
	0x151000, 0x160793, 0x170000, 0x189c23, 0x120550, 0x1324e7,
	0x140713, 0x151010, 0x169123, 0x1726e7, 0x18470d, 0x12055c,
	0x13c63a, 0x144702, 0x158d23, 0x162407, 0x17a223, 0x182607,
	0x120568, 0x130793, 0x140270, 0x150413, 0x160000, 0x171463,
	0x1800f7, 0x120574, 0x134789, 0x14c63e, 0x154709, 0x16cc3a,
	0x174702, 0x180793, 0x120580, 0x130270, 0x141463, 0x1500f7,
	0x16478d, 0x17cc3e, 0x180513, 0x12058c, 0x130000, 0x144792,
	0x154581, 0x164485, 0x179782, 0x184018, 0x120598, 0x131775,
	0x14e563, 0x1502e4, 0x162703, 0x170a04, 0x181163, 0x1205a4,
	0x130297, 0x144818, 0x150563, 0x160097, 0x1747a2, 0x18c804,
	0x1205b0, 0x139782, 0x1466b7, 0x150800, 0x16a703, 0x174c46,
	0x189b71, 0x1205bc, 0x136713, 0x140027, 0x15a223, 0x164ce6,
	0x174783, 0x180fd4, 0x1205c8, 0x13c7b9, 0x142683, 0x151004,
	0x164745, 0x179763, 0x1820e6, 0x1205d4, 0x133737, 0x140822,
	0x152683, 0x163007, 0x177645, 0x18167d, 0x1205e0, 0x138ef1,
	0x142023, 0x1530d7, 0x162683, 0x172807, 0x18e693, 0x1205ec,
	0x131006, 0x142023, 0x1528d7, 0x162683, 0x173807, 0x18e693,
	0x1205f8, 0x131006, 0x142023, 0x1538d7, 0x162683, 0x174007,
	0x18e693, 0x120604, 0x131006, 0x142023, 0x1540d7, 0x162683,
	0x174807, 0x18e693, 0x120610, 0x131006, 0x142023, 0x1548d7,
	0x1656b7, 0x170800, 0x18a703, 0x12061c, 0x133486, 0x14830d,
	0x158b05, 0x16cf01, 0x17a703, 0x185c46, 0x120628, 0x137671,
	0x14167d, 0x158f71, 0x166611, 0x17a223, 0x185ce6, 0x120634,
	0x138f51, 0x14a223, 0x155ce6, 0x162703, 0x171084, 0x1846b2,
	0x120640, 0x131c63, 0x1402d7, 0x153737, 0x160822, 0x172683,
	0x182807, 0x12064c, 0x13e693, 0x140016, 0x152023, 0x1628d7,
	0x172683, 0x183807, 0x120658, 0x13e693, 0x140016, 0x152023,
	0x1638d7, 0x172683, 0x184007, 0x120664, 0x13e693, 0x140016,
	0x152023, 0x1640d7, 0x172683, 0x184807, 0x120670, 0x13e693,
	0x140016, 0x152023, 0x1648d7, 0x172703, 0x181004, 0x12067c,
	0x1346b2, 0x149c63, 0x151ae6, 0x160737, 0x170800, 0x184b78,
	0x120688, 0x130693, 0x140ff0, 0x15463d, 0x168b1d, 0x17ce3a,
	0x1852b7, 0x120694, 0x130800, 0x144701, 0x154389, 0x16408d,
	0x174311, 0x180537, 0x1206a0, 0x130820, 0x141593, 0x150077,
	0x1695aa, 0x17418c, 0x184572, 0x1206ac, 0x1305c2, 0x1481c1,
	0x1581a9, 0x167763, 0x1700b5, 0x189533, 0x1206b8, 0x1300e4,
	0x144513, 0x15fff5, 0x168e69, 0x170537, 0x180800, 0x1206c4,
	0x134568, 0x148121, 0x15893d, 0x167463, 0x1702b5, 0x18a583,
	0x1206d0, 0x1306c2, 0x140763, 0x151277, 0x160a63, 0x171217,
	0x1805c2, 0x1206dc, 0x1381c1, 0x14818d, 0x150d63, 0x161097,
	0x178985, 0x180586, 0x1206e8, 0x1395b3, 0x1400b4, 0x15c593,
	0x16fff5, 0x178eed, 0x180705, 0x1206f4, 0x1315e3, 0x14fa67,
	0x1535b7, 0x160822, 0x17a703, 0x183005, 0x120700, 0x13757d,
	0x148a3d, 0x150513, 0x160ff5, 0x178f69, 0x180622, 0x12070c,
	0x138e59, 0x14a023, 0x1530c5, 0x168637, 0x170800, 0x185a38,
	0x120718, 0x1375c1, 0x14f693, 0x150ff6, 0x168593, 0x170ff5,
	0x188f6d, 0x120724, 0x1306a2, 0x148ed9, 0x15da34, 0x164682,
	0x170713, 0x180210, 0x120730, 0x139163, 0x140ee6, 0x154711,
	0x16e391, 0x17471d, 0x182023, 0x12073c, 0x1310e4, 0x142683,
	0x150a04, 0x16471d, 0x179e63, 0x1800e6, 0x120748, 0x136737,
	0x140800, 0x152703, 0x164cc7, 0x170693, 0x184000, 0x120754,
	0x137713, 0x144807, 0x151463, 0x1600d7, 0x172223, 0x180e04,
	0x120760, 0x134018, 0x141163, 0x150497, 0x165703, 0x1700c4,
	0x181793, 0x12076c, 0x130117, 0x14db63, 0x150207, 0x168737,
	0x170800, 0x184778, 0x120778, 0x137713, 0x140807, 0x15e705,
	0x160513, 0x170000, 0x184792, 0x120784, 0x134581, 0x149782,
	0x1547a2, 0x164711, 0x17c818, 0x18c004, 0x120790, 0x130d23,
	0x140094, 0x150ca3, 0x160004, 0x179782, 0x1856b7, 0x12079c,
	0x130800, 0x1442b8, 0x159b71, 0x16c2b8, 0x170513, 0x180000,
	0x1207a8, 0x1347d2, 0x149782, 0x154703, 0x162684, 0x1703e3,
	0x18de07, 0x1207b4, 0x13bbd9, 0x1407b7, 0x150002, 0x168793,
	0x1765c7, 0x18c83e, 0x1207c0, 0x1327b7, 0x140002, 0x158793,
	0x16dae7, 0x17c43e, 0x1847b7, 0x1207cc, 0x130002, 0x148793,
	0x151427, 0x16c23e, 0x1757b7, 0x180002, 0x1207d8, 0x138793,
	0x149867, 0x15b1fd, 0x162683, 0x171504, 0x184709, 0x1207e4,
	0x1399e3, 0x14e2e6, 0x1536b7, 0x160822, 0x17a703, 0x183006,
	0x1207f0, 0x13663d, 0x148f51, 0x15a023, 0x1630e6, 0x17bd39,
	0x18c593, 0x1207fc, 0x130015, 0x14b5dd, 0x158991, 0x1635b3,
	0x1700b0, 0x180589, 0x120808, 0x13bdf9, 0x148991, 0x15b593,
	0x160015, 0x17bfdd, 0x180737, 0x120814, 0x130800, 0x144f28,
	0x15cf89, 0x1647c2, 0x17893d, 0x189782, 0x120820, 0x1347e2,
	0x140713, 0x151000, 0x162223, 0x1710e4, 0x182423, 0x12082c,
	0x1310f4, 0x14474d, 0x15b729, 0x168111, 0x17b7dd, 0x1814e3,
	0x120838, 0x13f097, 0x140737, 0x150800, 0x164770, 0x171713,
	0x180106, 0x120844, 0x135d63, 0x140607, 0x1585b7, 0x160800,
	0x17a683, 0x180d05, 0x120850, 0x1372c5, 0x147313, 0x1500f6,
	0x1612fd, 0x17a703, 0x180d45, 0x12085c, 0x131513, 0x1400c3,
	0x15f6b3, 0x160056, 0x178ec9, 0x18757d, 0x120868, 0x130393,
	0x140ff5, 0x151293, 0x160083, 0x17f6b3, 0x180076, 0x120874,
	0x139b41, 0x148211, 0x15e2b3, 0x160056, 0x171093, 0x180043,
	0x120880, 0x137693, 0x140016, 0x156333, 0x160067, 0x170613,
	0x187ff5, 0x12088c, 0x139713, 0x1400b6, 0x157633, 0x1600c3,
	0x178e59, 0x189513, 0x120898, 0x1300a6, 0x147613, 0x159ff6,
	0x169713, 0x170096, 0x188e49, 0x1208a4, 0x13f293, 0x14f0f2,
	0x158e59, 0x16e2b3, 0x170012, 0x1806a2, 0x1208b0, 0x137613,
	0x14eff6, 0x158e55, 0x16a823, 0x170c55, 0x18aa23, 0x1208bc,
	0x130cc5, 0x1480e3, 0x15e807, 0x1646b7, 0x170822, 0x18a703,
	0x1208c8, 0x13f006, 0x149b61, 0x156713, 0x160027, 0x17a023,
	0x18f0e6, 0x1208d4, 0x13b5ad, 0x140000, 0x150000, 0x160000,
	0x170000, 0x180000, 0x110000, 0x120400, 0x104000, 0x1f0000,
};

static u16 patch_version1 = 0x9f73;
static u32 init_data1[] = {
	0x1f00a0, 0x1903f3, 0x1f0012, 0x150100, 0x1f00ad, 0x100000,
	0x11e0c6, 0x1f00a0, 0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb,
	0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb, 0x1f00ad, 0x110000,
	0x120400, 0x130093, 0x140000, 0x150193, 0x160000, 0x170213,
	0x180000, 0x12040c, 0x130293, 0x140000, 0x150313, 0x160000,
	0x170393, 0x180000, 0x120418, 0x130413, 0x140000, 0x150493,
	0x160000, 0x170513, 0x180000, 0x120424, 0x130593, 0x140000,
	0x150613, 0x160000, 0x170693, 0x180000, 0x120430, 0x130713,
	0x140000, 0x150793, 0x160000, 0x171137, 0x180000, 0x12043c,
	0x13006f, 0x140060, 0x15a001, 0x161111, 0x17cc06, 0x18ca22,
	0x120448, 0x13c826, 0x1417b7, 0x150800, 0x16aa23, 0x179407,
	0x180713, 0x120454, 0x1330f0, 0x1467b7, 0x150800, 0x16a423,
	0x1746e7, 0x18a703, 0x120460, 0x13a587, 0x146685, 0x158f55,
	0x16ac23, 0x17a4e7, 0x1867a9, 0x12046c, 0x135737, 0x140800,
	0x158793, 0x16f737, 0x172023, 0x1874f7, 0x120478, 0x1307b7,
	0x140800, 0x155bf8, 0x165793, 0x170037, 0x18f793, 0x120484,
	0x131f07, 0x147713, 0x1507f7, 0x168fd9, 0x170713, 0x180210,
	0x120490, 0x138763, 0x1400e7, 0x150713, 0x160270, 0x178263,
	0x181ce7, 0x12049c, 0x13a001, 0x141437, 0x150002, 0x160793,
	0x17e564, 0x18c43e, 0x1204a8, 0x1337b7, 0x140002, 0x158793,
	0x166867, 0x17c23e, 0x1847b7, 0x1204b4, 0x130002, 0x148793,
	0x15e9a7, 0x16c63e, 0x1767b7, 0x180800, 0x1204c0, 0x13a703,
	0x146d87, 0x1576c1, 0x168693, 0x170ff6, 0x188f75, 0x1204cc,
	0x1366b5, 0x148693, 0x158006, 0x168f55, 0x17ac23, 0x186ce7,
	0x1204d8, 0x13a703, 0x1465c7, 0x1556b7, 0x160800, 0x177713,
	0x18f0f7, 0x1204e4, 0x136713, 0x140807, 0x15ae23, 0x1664e7,
	0x17a703, 0x185c46, 0x1204f0, 0x130413, 0x140000, 0x159b75,
	0x16a223, 0x175ce6, 0x18a703, 0x1204fc, 0x13f5c7, 0x146691,
	0x158f55, 0x16ae23, 0x17f4e7, 0x180737, 0x120508, 0x130809,
	0x14433c, 0x158fd5, 0x16c33c, 0x170793, 0x180000, 0x120514,
	0x130713, 0x141000, 0x159c23, 0x1624e7, 0x170713, 0x181010,
	0x120520, 0x138d23, 0x142407, 0x159123, 0x1626e7, 0x17a223,
	0x182607, 0x12052c, 0x13c026, 0x144782, 0x154581, 0x164485,
	0x170513, 0x180000, 0x120538, 0x134792, 0x149782, 0x154018,
	0x161775, 0x17e563, 0x1802e4, 0x120544, 0x132703, 0x140a04,
	0x151163, 0x160297, 0x174818, 0x180563, 0x120550, 0x130097,
	0x1447a2, 0x15c804, 0x169782, 0x176637, 0x180800, 0x12055c,
	0x132703, 0x144c46, 0x159b71, 0x166713, 0x170027, 0x182223,
	0x120568, 0x134ce6, 0x144703, 0x150fd4, 0x16c739, 0x172603,
	0x181004, 0x120574, 0x134745, 0x141263, 0x1510e6, 0x163737,
	0x170822, 0x182603, 0x120580, 0x133007, 0x1475c5, 0x1515fd,
	0x168e6d, 0x172023, 0x1830c7, 0x12058c, 0x132603, 0x142807,
	0x156613, 0x161006, 0x172023, 0x1828c7, 0x120598, 0x132603,
	0x143807, 0x156613, 0x161006, 0x172023, 0x1838c7, 0x1205a4,
	0x132603, 0x144007, 0x156613, 0x161006, 0x172023, 0x1840c7,
	0x1205b0, 0x132603, 0x144807, 0x156613, 0x161006, 0x172023,
	0x1848c7, 0x1205bc, 0x135637, 0x140800, 0x152703, 0x163486,
	0x17830d, 0x188b05, 0x1205c8, 0x13cf01, 0x142703, 0x155c46,
	0x1675f1, 0x1715fd, 0x188f6d, 0x1205d4, 0x136591, 0x142223,
	0x155ce6, 0x168f4d, 0x172223, 0x185ce6, 0x1205e0, 0x132603,
	0x140a04, 0x15471d, 0x161e63, 0x1700e6, 0x186737, 0x1205ec,
	0x130800, 0x142703, 0x154cc7, 0x160613, 0x174000, 0x187713,
	0x1205f8, 0x134807, 0x141463, 0x1500c7, 0x162223, 0x170e04,
	0x184018, 0x120604, 0x131263, 0x140497, 0x155703, 0x1600c4,
	0x171793, 0x180117, 0x120610, 0x13dc63, 0x140207, 0x158737,
	0x160800, 0x174778, 0x187713, 0x12061c, 0x130807, 0x14e70d,
	0x154782, 0x164581, 0x170513, 0x180000, 0x120628, 0x134792,
	0x149782, 0x1547a2, 0x164711, 0x17c818, 0x18c004, 0x120634,
	0x130d23, 0x140094, 0x150ca3, 0x160004, 0x179782, 0x185637,
	0x120640, 0x130800, 0x144238, 0x159b71, 0x16c238, 0x174782,
	0x180513, 0x12064c, 0x130000, 0x1447b2, 0x159782, 0x164703,
	0x172684, 0x1803e3, 0x120658, 0x13ee07, 0x14bdd1, 0x152437,
	0x160002, 0x170793, 0x18dae4, 0x120664, 0x13c43e, 0x1447b7,
	0x150002, 0x168793, 0x171427, 0x18c23e, 0x120670, 0x1357b7,
	0x140002, 0x158793, 0x169867, 0x17b589, 0x182603, 0x12067c,
	0x131504, 0x144709, 0x151ee3, 0x16f2e6, 0x173637, 0x180822,
	0x120688, 0x132703, 0x143006, 0x1565bd, 0x168f4d, 0x172023,
	0x1830e6, 0x120694, 0x13b725, 0x140000, 0x150000, 0x160000,
	0x170000, 0x180000, 0x110000, 0x120400, 0x104000, 0x1f0000,
};

static u16 patch_version2 = 0x2e9c;
static u32 init_data2[] = {
	0x1f00a0, 0x1903f3, 0x1f0012, 0x150100, 0x1f00ad, 0x100000,
	0x11e0c6, 0x1f0102, 0x199000, 0x1f00a0, 0x1903fb, 0x1903fb,
	0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb, 0x1903fb,
	0x1f00ad, 0x110000, 0x121000, 0x13937c, 0x140120, 0x15675e,
	0x16fca8, 0x1706b7, 0x180800, 0x12100c, 0x135af8, 0x145793,
	0x150037, 0x16f793, 0x171f07, 0x187713, 0x121018, 0x1307f7,
	0x148fd9, 0x150713, 0x1606a0, 0x179b63, 0x1808e7, 0x121024,
	0x13cd19, 0x141121, 0x15c822, 0x16ca06, 0x17c626, 0x184789,
	0x121030, 0x13842e, 0x140963, 0x152cf5, 0x1640d2, 0x174442,
	0x1844b2, 0x12103c, 0x134501, 0x140161, 0x158082, 0x1617b7,
	0x170800, 0x18aa23, 0x121048, 0x139407, 0x1467b7, 0x150800,
	0x16a703, 0x17a587, 0x186605, 0x121054, 0x138f51, 0x14ac23,
	0x15a4e7, 0x16670d, 0x170713, 0x18e9c7, 0x121060, 0x135637,
	0x140800, 0x152023, 0x1674e6, 0x174ab8, 0x186713, 0x12106c,
	0x130407, 0x14cab8, 0x15a703, 0x164d47, 0x1776fd, 0x188693,
	0x121078, 0x1303f6, 0x148f75, 0x156713, 0x162807, 0x17aa23,
	0x184ce7, 0x121084, 0x13a703, 0x147587, 0x157713, 0x16e1f7,
	0x176713, 0x180607, 0x121090, 0x13ac23, 0x1474e7, 0x15a703,
	0x164f47, 0x177713, 0x18f0f7, 0x12109c, 0x136713, 0x140107,
	0x15aa23, 0x164ee7, 0x17a703, 0x18fc07, 0x1210a8, 0x139b5d,
	0x14a023, 0x15fce7, 0x160713, 0x170300, 0x18a223, 0x1210b4,
	0x13a6e7, 0x144501, 0x158082, 0x166489, 0x179023, 0x180004,
	0x1210c0, 0x134505, 0x142ebd, 0x1557fd, 0x16c49c, 0x17c4dc,
	0x184783, 0x1210cc, 0x130ec4, 0x149363, 0x152407, 0x16b795,
	0x1787b7, 0x180800, 0x1210d8, 0x13a583, 0x140dc7, 0x15a783,
	0x160e07, 0x178b91, 0x188063, 0x1210e4, 0x131807, 0x14d793,
	0x150085, 0x1681b9, 0x178bbd, 0x188985, 0x1210f0, 0x1386b7,
	0x140800, 0x15a703, 0x160d06, 0x17833e, 0x189513, 0x1210fc,
	0x1300c7, 0x1477c5, 0x1517fd, 0x168f7d, 0x178f49, 0x18757d,
	0x121108, 0x130293, 0x140ff5, 0x15a603, 0x160d46, 0x171793,
	0x180083, 0x121114, 0x137733, 0x140057, 0x158f5d, 0x167713,
	0x17f0f7, 0x181793, 0x121120, 0x130043, 0x148f5d, 0x157793,
	0x16ff06, 0x170513, 0x187ff5, 0x12112c, 0x13e7b3, 0x140067,
	0x159613, 0x1600b5, 0x178fe9, 0x188fd1, 0x121138, 0x139513,
	0x1400a5, 0x15f793, 0x169ff7, 0x179613, 0x180095, 0x121144,
	0x138fc9, 0x148fd1, 0x1505a2, 0x16f793, 0x17eff7, 0x188fcd,
	0x121150, 0x13a823, 0x140ce6, 0x15aa23, 0x160cf6, 0x172783,
	0x180f04, 0x12115c, 0x134719, 0x148563, 0x1500e7, 0x16472d,
	0x179263, 0x1810e7, 0x121168, 0x130493, 0x141104, 0x154701,
	0x164781, 0x17d683, 0x180104, 0x121174, 0x13e2b5, 0x1446a1,
	0x154701, 0x16853e, 0x17c436, 0x18c23a, 0x121180, 0x13c03e,
	0x142ca9, 0x1546a2, 0x164712, 0x174782, 0x1816fd, 0x12118c,
	0x13972a, 0x14f6f5, 0x150637, 0x160820, 0x175693, 0x184037,
	0x121198, 0x139713, 0x140077, 0x159732, 0x164310, 0x17d703,
	0x180004, 0x1211a4, 0x139963, 0x1422e6, 0x159593, 0x160017,
	0x176709, 0x18972e, 0x1211b0, 0x135703, 0x140087, 0x157e63,
	0x1600e6, 0x17d703, 0x180184, 0x1211bc, 0x139023, 0x1400d4,
	0x159693, 0x160017, 0x179423, 0x1800e4, 0x1211c8, 0x136709,
	0x149736, 0x151423, 0x1600c7, 0x17d703, 0x180184, 0x1211d4,
	0x133713, 0x140017, 0x150785, 0x164691, 0x170489, 0x1899e3,
	0x1211e0, 0x13f8d7, 0x1447b5, 0x151f63, 0x161807, 0x176785,
	0x1897a2, 0x1211ec, 0x13a703, 0x14a687, 0x154791, 0x160b63,
	0x1718f7, 0x186489, 0x1211f8, 0x13d783, 0x140004, 0x159de3,
	0x16e207, 0x172703, 0x180f04, 0x121204, 0x134789, 0x1418e3,
	0x15e2f7, 0x164505, 0x172c15, 0x185637, 0x121210, 0x130800,
	0x1446b7, 0x150822, 0x164238, 0x17a783, 0x18f806, 0x12121c,
	0x133537, 0x140822, 0x1575c1, 0x16f793, 0x178ff7, 0x18e793,
	0x121228, 0x133007, 0x14a023, 0x15f8f6, 0x162683, 0x173805,
	0x188593, 0x121234, 0x130ff5, 0x14d793, 0x150086, 0x168b85,
	0x17e793, 0x180767, 0x121240, 0x138eed, 0x1407a2, 0x158fd5,
	0x162023, 0x1738f5, 0x187793, 0x12124c, 0x13ffd7, 0x14c23c,
	0x15e793, 0x160027, 0x17c23c, 0x1847c1, 0x121258, 0x139023,
	0x1400f4, 0x154501, 0x1622cd, 0x17bbd9, 0x184585, 0x121264,
	0x134791, 0x14b569, 0x15476d, 0x169fe3, 0x17f6e7, 0x180737,
	0x121270, 0x130002, 0x144481, 0x150713, 0x160a67, 0x178793,
	0x184b84, 0x12127c, 0x130786, 0x1497a2, 0x15d683, 0x160ee7,
	0x178526, 0x180485, 0x121288, 0x13e693, 0x140026, 0x159723,
	0x160ed7, 0x179702, 0x180737, 0x121294, 0x130002, 0x144791,
	0x150713, 0x160a67, 0x179ee3, 0x18fcf4, 0x1212a0, 0x130493,
	0x141000, 0x154501, 0x1614fd, 0x172a15, 0x18fced, 0x1212ac,
	0x134785, 0x148526, 0x15c03e, 0x16222d, 0x17470d, 0x184782,
	0x1212b8, 0x135363, 0x1400a7, 0x154781, 0x160485, 0x174711,
	0x1896e3, 0x1212c4, 0x13fee4, 0x14c38d, 0x156789, 0x164749,
	0x179023, 0x1800e7, 0x1212d0, 0x1347c5, 0x142c23, 0x150ef4,
	0x160793, 0x171000, 0x182a23, 0x1212dc, 0x130ef4, 0x1445d1,
	0x150513, 0x160ec4, 0x172a15, 0x18b709, 0x1212e8, 0x136705,
	0x149722, 0x155783, 0x16a5a7, 0x1746a1, 0x180785, 0x1212f4,
	0x1307c2, 0x1483c1, 0x15f963, 0x1602f6, 0x171d23, 0x18a407,
	0x121300, 0x132823, 0x140e04, 0x154783, 0x160ec4, 0x1789e3,
	0x18da07, 0x12130c, 0x132783, 0x140f04, 0x1585e3, 0x16da07,
	0x172783, 0x180f04, 0x121318, 0x138ee3, 0x14da07, 0x154741,
	0x169de3, 0x17e2e7, 0x1847ed, 0x121324, 0x132823, 0x140ef4,
	0x15bd05, 0x1606b7, 0x170002, 0x181d23, 0x121330, 0x13a4f7,
	0x144481, 0x158693, 0x160a66, 0x178793, 0x184b84, 0x12133c,
	0x130786, 0x1497a2, 0x15d703, 0x160ee7, 0x178526, 0x189b75,
	0x121348, 0x139723, 0x140ee7, 0x159682, 0x160737, 0x170820,
	0x189793, 0x121354, 0x130074, 0x140713, 0x152007, 0x1697ba,
	0x17a023, 0x180007, 0x121360, 0x130737, 0x140002, 0x150485,
	0x164791, 0x170693, 0x180a67, 0x12136c, 0x1396e3, 0x14fcf4,
	0x150737, 0x160828, 0x172783, 0x184807, 0x121378, 0x13e793,
	0x140207, 0x152023, 0x1648f7, 0x1747b9, 0x182823, 0x121384,
	0x130ef4, 0x14b58d, 0x156709, 0x165683, 0x170007, 0x1847c9,
	0x121390, 0x1393e3, 0x14e6f6, 0x1547b7, 0x160800, 0x17a783,
	0x180c87, 0x12139c, 0x130693, 0x141000, 0x1507c2, 0x1683c1,
	0x17f663, 0x1802f6, 0x1213a8, 0x135783, 0x140027, 0x150785,
	0x1607c2, 0x1783c1, 0x181123, 0x1213b4, 0x1300f7, 0x144725,
	0x157fe3, 0x16c6f7, 0x1766b7, 0x180800, 0x1213c0, 0x13a783,
	0x14f5c6, 0x157779, 0x16177d, 0x178ff9, 0x18ae23, 0x1213cc,
	0x13f4f6, 0x14b1a5, 0x151123, 0x160007, 0x17b18d, 0x18c1e3,
	0x1213d8, 0x13dee6, 0x14bbdd, 0x150737, 0x160830, 0x172783,
	0x181807, 0x1213e4, 0x132703, 0x142007, 0x154685, 0x160263,
	0x1702d5, 0x18c763, 0x1213f0, 0x1300a6, 0x148bfd, 0x15c111,
	0x164781, 0x17853e, 0x188082, 0x1213fc, 0x134689, 0x140b63,
	0x1500d5, 0x16478d, 0x1719e3, 0x18fef5, 0x121408, 0x135793,
	0x140047, 0x15a011, 0x168395, 0x178bfd, 0x18b7dd, 0x121414,
	0x1383a9, 0x14bfed, 0x151141, 0x16c422, 0x17c606, 0x1847d1,
	0x121420, 0x13842a, 0x149a63, 0x1500f5, 0x164508, 0x1707b7,
	0x180002, 0x12142c, 0x13c02e, 0x148793, 0x156687, 0x169782,
	0x174582, 0x18c04c, 0x121438, 0x1340b2, 0x144422, 0x150141,
	0x168082, 0x17ed09, 0x184781, 0x121444, 0x134737, 0x140822,
	0x152023, 0x16d0f7, 0x172023, 0x18d8f7, 0x121450, 0x132023,
	0x14e0f7, 0x152023, 0x16e8f7, 0x178082, 0x186785, 0x12145c,
	0x133737, 0x140822, 0x158693, 0x16fff7, 0x172023, 0x1830d7,
	0x121468, 0x132023, 0x145007, 0x152023, 0x165807, 0x172023,
	0x186007, 0x121474, 0x132023, 0x146807, 0x158793, 0x16c007,
	0x17b7e1, 0x180000, 0x1f00a0, 0x1903f3, 0x1903fb, 0x1f0000,
};

int jl2xxx_patch_static_op_set(struct phy_device *phydev)
{
	int err;

	err = jl2xxx_pre_init(phydev, init_data0, ARRAY_SIZE(init_data0),
			      patch_fw_versions0,
			      ARRAY_SIZE(patch_fw_versions0), patch_version0);
	if (err < 0)
		return err;

	err = jl2xxx_pre_init(phydev, init_data1, ARRAY_SIZE(init_data1),
			      patch_fw_versions1,
			      ARRAY_SIZE(patch_fw_versions1), patch_version1);
	if (err < 0)
		return err;

	err = jl2xxx_pre_init(phydev, init_data2, ARRAY_SIZE(init_data2),
			      patch_fw_versions2,
			      ARRAY_SIZE(patch_fw_versions2), patch_version2);
	if (err < 0)
		return err;

	return 0;
}

#if (JLSEMI_PHY_WOL)
int jl1xxx_wol_dynamic_op_get(struct phy_device *phydev)
{
	return jlsemi_fetch_bit(phydev, JL1XXX_PAGE129,
				JL1XXX_WOL_CTRL_REG, JL1XXX_WOL_DIS);
}

int jl2xxx_wol_dynamic_op_get(struct phy_device *phydev)
{
	return jlsemi_fetch_bit(phydev, JL2XXX_WOL_CTRL_PAGE,
				JL2XXX_WOL_CTRL_REG, JL2XXX_WOL_EN);
}

static int jl1xxx_wol_static_op_set(struct phy_device *phydev)
{
	int err;

	err = jl1xxx_wol_dynamic_op_set(phydev);
	if (err < 0)
		return err;

	return 0;
}
#endif

int jl1xxx_intr_ack_event(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->intr.enable & JL1XXX_INTR_STATIC_OP_EN) {
		err = phy_read(phydev, JL1XXX_INTR_STATUS_REG);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl1xxx_intr_static_op_set(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;
	int ret = 0;

	if (priv->intr.enable & JL1XXX_INTR_LINK_CHANGE_EN)
		ret |= JL1XXX_INTR_LINK;
	if (priv->intr.enable & JL1XXX_INTR_AN_ERR_EN)
		ret |= JL1XXX_INTR_AN_ERR;

	err = jlsemi_set_bits(phydev, JL1XXX_PAGE7,
			      JL1XXX_INTR_REG, ret);
	if (err < 0)
		return err;

	return 0;
}

#if (JLSEMI_PHY_WOL)
static int jl2xxx_wol_static_op_set(struct phy_device *phydev)
{
	int err;

	err = jl2xxx_wol_dynamic_op_set(phydev);
	if (err < 0)
		return err;

	return 0;
}

int jl1xxx_wol_dynamic_op_set(struct phy_device *phydev)
{
	int err;

	err = jl1xxx_wol_cfg_rmii(phydev);
	if (err < 0)
		return err;

	err = jl1xxx_wol_enable(phydev, true);
	if (err < 0)
		return err;

	err = jl1xxx_wol_store_mac_addr(phydev);
	if (err < 0)
		return err;

	if (jl1xxx_wol_receive_check(phydev)) {
		err = jl1xxx_wol_clear(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl2xxx_wol_dynamic_op_set(struct phy_device *phydev)
{
	int err;

	err = jl2xxx_wol_enable(phydev, true);
	if (err < 0)
		return err;

	err = jl2xxx_wol_clear(phydev);
	if (err < 0)
		return err;

	err = jl2xxx_wol_active_low_polarity(phydev, true);
	if (err < 0)
		return err;

	err = jl2xxx_store_mac_addr(phydev);
	if (err < 0)
		return err;

	return 0;
}
#endif

int jl2xxx_intr_ack_event(struct phy_device *phydev)
{
	int err;

	err = jlsemi_read_paged(phydev, JL2XXX_PAGE2627,
				JL2XXX_INTR_STATUS_REG);
	if (err < 0)
		return err;

	return 0;
}

int jl2xxx_intr_static_op_set(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;
	int ret = 0;

	if (priv->intr.enable & JL2XXX_INTR_LINK_CHANGE_EN)
		ret |= JL2XXX_INTR_LINK_CHANGE;
	if (priv->intr.enable & JL2XXX_INTR_AN_ERR_EN)
		ret |= JL2XXX_INTR_AN_ERR;
	if (priv->intr.enable & JL2XXX_INTR_AN_COMPLETE_EN)
		ret |= JL2XXX_INTR_AN_COMPLETE;
	if (priv->intr.enable & JL2XXX_INTR_AN_PAGE_RECE)
		ret |= JL2XXX_INTR_AN_PAGE;

	err = jlsemi_set_bits(phydev, JL2XXX_PAGE2626,
			      JL2XXX_INTR_CTRL_REG, ret);
	if (err < 0)
		return err;

	err = jlsemi_set_bits(phydev, JL2XXX_PAGE158,
			      JL2XXX_INTR_PIN_REG,
			      JL2XXX_INTR_PIN_EN);
	if (err < 0)
		return err;

	err = jlsemi_set_bits(phydev, JL2XXX_PAGE160,
			      JL2XXX_PIN_EN_REG,
			      JL2XXX_PIN_OUTPUT);
	if (err < 0)
		return err;

	return 0;
}

int jl1xxx_operation_args_get(struct phy_device *phydev)
{
	jl1xxx_led_operation_args(phydev);
	jl1xxx_wol_operation_args(phydev);
	jl1xxx_intr_operation_args(phydev);
	jl1xxx_mdi_operation_args(phydev);
	jl1xxx_rmii_operation_args(phydev);

	return 0;
}

int jl2xxx_operation_args_get(struct phy_device *phydev)
{
	jl2xxx_led_operation_args(phydev);
	jl2xxx_fld_operation_args(phydev);
	jl2xxx_wol_operation_args(phydev);
	jl2xxx_intr_operation_args(phydev);
	jl2xxx_downshift_operation_args(phydev);
	jl2xxx_rgmii_operation_args(phydev);
	jl2xxx_patch_operation_args(phydev);
	jl2xxx_clk_operation_args(phydev);
	jl2xxx_work_mode_operation_args(phydev);
	jl2xxx_lpbk_operation_args(phydev);
	jl2xxx_slew_rate_operation_args(phydev);
	jl2xxx_rxc_out_operation_args(phydev);

	return 0;
}

int jl1xxx_static_op_init(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->led.enable & JL1XXX_LED_STATIC_OP_EN) {
		err = jl1xxx_led_static_op_set(phydev);
		if (err < 0)
			return err;
	}

#if (JLSEMI_PHY_WOL)
	if (priv->wol.enable & JL1XXX_WOL_STATIC_OP_EN) {
		err = jl1xxx_wol_static_op_set(phydev);
		if (err < 0)
			return err;
	}
#endif

	if (priv->intr.enable & JL1XXX_INTR_STATIC_OP_EN) {
		err = jl1xxx_intr_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->mdi.enable & JL1XXX_MDI_STATIC_OP_EN) {
		err = jl1xxx_mdi_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->rmii.enable & JL1XXX_RMII_STATIC_OP_EN) {
		err = jl1xxx_rmii_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl2xxx_static_op_init(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->patch.enable & JL2XXX_PATCH_STATIC_OP_EN) {
		err = jl2xxx_patch_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->led.enable & JL2XXX_LED_STATIC_OP_EN) {
		err = jl2xxx_led_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->fld.enable & JL2XXX_FLD_STATIC_OP_EN) {
		err = jl2xxx_fld_static_op_set(phydev);
		if (err < 0)
			return err;
	}

#if (JLSEMI_PHY_WOL)
	if (priv->wol.enable & JL2XXX_WOL_STATIC_OP_EN) {
		err = jl2xxx_wol_static_op_set(phydev);
		if (err < 0)
			return err;
	}
#endif

	if (priv->intr.enable & JL2XXX_INTR_STATIC_OP_EN) {
		err = jl2xxx_intr_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->downshift.enable & JL2XXX_DSFT_STATIC_OP_EN) {
		err = jl2xxx_downshift_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->rgmii.enable & JL2XXX_RGMII_STATIC_OP_EN) {
		err = jl2xxx_rgmii_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->clk.enable & JL2XXX_CLK_STATIC_OP_EN) {
		err = jl2xxx_clk_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->work_mode.enable & JL2XXX_WORK_MODE_STATIC_OP_EN) {
		err = jl2xxx_work_mode_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->lpbk.enable & JL2XXX_LPBK_STATIC_OP_EN) {
		err = jl2xxx_lpbk_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->slew_rate.enable & JL2XXX_SLEW_RATE_STATIC_OP_EN) {
		err = jl2xxx_slew_rate_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	if (priv->rxc_out.enable & JL2XXX_RXC_OUT_STATIC_OP_EN) {
		err = jl2xxx_rxc_out_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

int jl2xxx_pre_init(struct phy_device *phydev,
		    u32 *init_data, u16 data_len,
		    u16 *patch_fw_versions, u16 versions_len,
		    u16 patch_version)
{
	int i, j;
	int regaddr, val;
	bool patch_ok = false;

	val = jlsemi_read_paged(phydev, JL2XXX_PAGE0, JL2XXX_PHY_INFO_REG);
	for (i = 0; i < versions_len; i++) {
		if (val == patch_fw_versions[i])
			patch_ok |= true;
	}

	if (!patch_ok)
		return 0;

	for (i = 0; i < data_len; i++) {
		regaddr = ((init_data[i] >> 16) & 0xff);
		val = (init_data[i] & 0xffff);
		phy_write(phydev, regaddr, val);
		if (regaddr == 0x18) {
			phy_write(phydev, 0x10, 0x8006);
			for (j = 0; j < 8; j++) {
				if (phy_read(phydev, 0x10) == 0)
					break;
			}
		}
	}
	/* Wait load patch complete */
	msleep(20);

	val = jlsemi_read_paged(phydev, JL2XXX_PAGE174, JL2XXX_PATCH_REG);

	if (val != patch_version)
		JLSEMI_PHY_MSG(KERN_ERR
			       "%s: patch version is not match\n", __func__);

	return 0;
}

int jlsemi_soft_reset(struct phy_device *phydev)
{
	int err;

	err = jlsemi_set_bits(phydev, JL2XXX_PAGE0,
			      JL2XXX_BMCR_REG, JL2XXX_SOFT_RESET);
	if (err < 0)
		return err;
	/* Wait soft reset complete*/
	msleep(600);

	return 0;
}

/********************** Convenience function for phy **********************/

/**
 * jlsemi_write_page() - write the page register
 * @phydev: a pointer to a &struct phy_device
 * @page: page values
 */
int jlsemi_write_page(struct phy_device *phydev, int page)
{
	return phy_write(phydev, JLSEMI_PAGE31, page);
}

/**
 * jlsemi_read_page() - write the page register
 * @phydev: a pointer to a &struct phy_device
 *
 * Return: get page values at present
 */
int jlsemi_read_page(struct phy_device *phydev)
{
	return phy_read(phydev, JLSEMI_PAGE31);
}

/**
 * __jlsemi_save_page() - save the page value
 *@phydev: a pointer to a &struct phy_device
 *
 * Return: save page value
 */
static inline int __jlsemi_save_page(struct phy_device *phydev)
{
	return jlsemi_read_page(phydev);
}

/**
 * __jlsemi_select_page() - restore the page register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page
 *
 * Return:
 * @oldpgae: this is last page value
 * @ret: if page is change it will return new page value
 */
static inline int __jlsemi_select_page(struct phy_device *phydev, int page)
{
	int ret, oldpage;

	oldpage = ret = __jlsemi_save_page(phydev);
	if (ret < 0)
		return ret;

	if (oldpage != page) {
		ret = jlsemi_write_page(phydev, page);
		if (ret < 0)
			return ret;
	}

	return oldpage;
}

/**
 * __jlsemi_restore_page() - restore the page register
 * @phydev: a pointer to a &struct phy_device
 * @oldpage: the old page, return value from __jlsemi_save_page() or
 * __jlsemi_select_page()
 * @ret: operation's return code
 *
 * Returns:
 *   @oldpage if it was a negative value, otherwise
 *   @ret if it was a negative errno value, otherwise
 *   phy_write_page()'s negative value if it were in error, otherwise
 *   @ret
 */
static inline int __jlsemi_restore_page(struct phy_device *phydev,
					int oldpage, int ret)
{
	int r;

	if (oldpage >= 0) {
		r = jlsemi_write_page(phydev, oldpage);

		/* Propagate the operation return code if the page write
		 * was successful.
		 */
		if (ret >= 0 && r < 0)
			ret = r;
	} else {
		/* Propagate the phy page selection error code */
		ret = oldpage;
	}

	return ret;
}

/**
 * __jlsemi_modify_reg() - Convenience function for modifying a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @regnum: register number
 * @mask: bit mask of bits to clear
 * @set: bit mask of bits to set
 *
 * Returns negative errno, 0 if there was no change, and 1 in case of change
 */
static inline int __jlsemi_modify_reg(struct phy_device *phydev,
				      u32 regnum, u16 mask, u16 set)
{
	int newval, ret;

	ret = phy_read(phydev, regnum);
	if (ret < 0)
		return ret;

	newval = (ret & ~mask) | set;
	if (newval == ret)
		return 0;

	ret = phy_write(phydev, regnum, newval);

	return ret < 0 ? ret : 1;
}

/**
 * jlsemi_modify_paged_reg() - Function for modifying a paged register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number
 * @mask: bit mask of bits to clear
 * @set: bit mask of bits to set
 *
 * Returns negative errno, 0 if there was no change, and 1 in case of change
 */
int jlsemi_modify_paged_reg(struct phy_device *phydev,
			    int page, u32 regnum,
			    u16 mask, u16 set)
{
	int ret = 0, oldpage;

	oldpage = __jlsemi_select_page(phydev, page);
	if (oldpage >= 0)
		ret = __jlsemi_modify_reg(phydev, regnum, mask, set);

	return __jlsemi_restore_page(phydev, oldpage, ret);
}

/**
 * jlsemi_set_bits() - Convenience function for setting bits in a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bits to set
 */
int jlsemi_set_bits(struct phy_device *phydev,
		    int page, u32 regnum, u16 val)
{
	return jlsemi_modify_paged_reg(phydev, page, regnum, 0, val);
}

/**
 * jlsemi_clear_bits - Convenience function for clearing bits in a PHY register
 * @phydev: the phy_device struct
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bits to clear
 */
int jlsemi_clear_bits(struct phy_device *phydev,
		      int page, u32 regnum, u16 val)
{
	return jlsemi_modify_paged_reg(phydev, page, regnum, val, 0);
}

/**
 * jlsemi_fetch_bit() - Convenience function for setting bits in a PHY register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number to write
 * @val: bit to get
 *
 * Note:
 * you only get one bit at meanwhile
 *
 */
int jlsemi_fetch_bit(struct phy_device *phydev,
		   int page, u32 regnum, u16 val)
{
	int ret = 0, oldpage;

	oldpage = __jlsemi_select_page(phydev, page);
	if (oldpage >= 0) {
		ret = phy_read(phydev, regnum);
		if (ret < 0)
			return ret;
		ret = ((ret & val) == val) ? 1 : 0;
	}

	return __jlsemi_restore_page(phydev, oldpage, ret);
}

/**
 * jlsemi_read_paged() - Convenience function for reading a paged register
 * @phydev: a pointer to a &struct phy_device
 * @page: the page for the phy
 * @regnum: register number
 *
 * Same rules as for phy_read().
 */
int jlsemi_read_paged(struct phy_device *phydev, int page, u32 regnum)
{
	int ret = 0, oldpage;

	oldpage = __jlsemi_select_page(phydev, page);
	if (oldpage >= 0)
		ret = phy_read(phydev, regnum);

	return __jlsemi_restore_page(phydev, oldpage, ret);
}

#if (KERNEL_VERSION(4, 0, 0) > LINUX_VERSION_CODE)
int jlsemi_drivers_register(struct phy_driver *phydrvs, int size)
{
	int i, j;
	int ret;

	for (i = 0; i < size; i++) {
		ret = phy_driver_register(&phydrvs[i]);
		if (ret)
			goto err;
	}

	return 0;

err:
	for (j = 0; j < i; j++)
		phy_driver_unregister(&phydrvs[j]);

	return ret;
}

void jlsemi_drivers_unregister(struct phy_driver *phydrvs, int size)
{
	int i;

	for (i = 0; i < size; i++)
		phy_driver_unregister(&phydrvs[i]);
}
#else
#endif
