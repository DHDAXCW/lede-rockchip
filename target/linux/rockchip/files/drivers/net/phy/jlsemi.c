// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/net/phy/jlsemi.c
 *
 * Driver for JLSemi PHYs
 *
 * Author: Gangqiao Kuang <gqkuang@jlsemi.com>
 *
 * Copyright (c) 2021 JingLue Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include "jlsemi-core.h"
#include <linux/phy.h>
#include <linux/module.h>
#include <linux/netdevice.h>

#define DRIVER_VERSION		"1.2.9"
#define DRIVER_NAME_100M	"JL1xxx Fast Ethernet " DRIVER_VERSION
#define DRIVER_NAME_1000M	"JL2xxx Gigabit Ethernet " DRIVER_VERSION

MODULE_DESCRIPTION("JLSemi PHY driver");
MODULE_AUTHOR("Gangqiao Kuang");
MODULE_LICENSE("GPL");

#if (JLSEMI_DEBUG_INFO)
static int jlsemi_phy_reg_print(struct phy_device *phydev)
{
	int ret;
	int i;

	for (i = 0; i < ARRAY_SIZE(jl_phy); i++) {
		if (jl_phy[i].enable) {
			ret = jlsemi_read_paged(phydev, jl_phy[i].page,
						jl_phy[i].reg);
			JLSEMI_PHY_MSG("%s: 0x%x\n", jl_phy[i].string, ret);
			if (ret < 0)
				return ret;
		}
	}

	return 0;
}
#endif

static int jl1xxx_probe(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl1xxx_priv *jl1xxx = NULL;
	int err;

	jl1xxx = devm_kzalloc(dev, sizeof(*jl1xxx), GFP_KERNEL);
	if (!jl1xxx)
		return -ENOMEM;

	phydev->priv = jl1xxx;

#if (JLSEMI_KERNEL_DEVICE_TREE_USE)
	if (!dev->of_node)
		JLSEMI_PHY_MSG("%s: Find device node failed\n", __func__);
#endif
	err = jl1xxx_operation_args_get(phydev);
	if (err < 0)
		return err;

	if (jl1xxx->intr.enable & JL1XXX_INTR_STATIC_OP_EN)
		phydev->irq = JL1XXX_INTR_IRQ;

	jl1xxx->static_inited = false;

	return 0;
}

static int jl1xxx_config_init(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int ret;

	if (!priv->static_inited) {
#if (JLSEMI_DEBUG_INFO)
		JLSEMI_PHY_MSG("jl1xxx_config_init_before:\n");
		ret = jlsemi_phy_reg_print(phydev);
		if (ret < 0)
			return ret;
#endif
		ret = jl1xxx_static_op_init(phydev);
		if (ret < 0)
			return ret;
#if (JLSEMI_DEBUG_INFO)
		JLSEMI_PHY_MSG("jl1xxx_config_init_after:\n");
		ret = jlsemi_phy_reg_print(phydev);
		if (ret < 0)
			return ret;
#endif
		priv->static_inited = JLSEMI_PHY_NOT_REENTRANT;
	}

	return 0;
}

static int jl1xxx_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = jl1xxx_intr_ack_event(phydev);
	if (err < 0)
		return err;

	return 0;
}

static int jl1xxx_config_intr(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->intr.enable & JL1XXX_INTR_STATIC_OP_EN) {
		err = jl1xxx_ack_interrupt(phydev);
		if (err < 0)
			return err;

		err = jl1xxx_intr_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

static int jl1xxx_read_status(struct phy_device *phydev)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->intr.enable & JL1XXX_INTR_STATIC_OP_EN) {
		err = jl1xxx_ack_interrupt(phydev);
		if (err < 0)
			return err;
	}

	return genphy_read_status(phydev);
}

static void jl1xxx_remove(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl1xxx_priv *priv = phydev->priv;

	if (priv)
		devm_kfree(dev, priv);
}

#if (JLSEMI_PHY_WOL)
static void jl1xxx_get_wol(struct phy_device *phydev,
			   struct ethtool_wolinfo *wol)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int wol_en;

	if (priv->wol.ethtool) {
		wol->supported = WAKE_MAGIC;
		wol->wolopts = 0;

		wol_en = jl1xxx_wol_dynamic_op_get(phydev);

		if (!wol_en)
			wol->wolopts |= WAKE_MAGIC;
	}
}

static int jl1xxx_set_wol(struct phy_device *phydev,
			  struct ethtool_wolinfo *wol)
{
	struct jl1xxx_priv *priv = phydev->priv;
	int err;

	if (priv->wol.ethtool) {
		if (wol->wolopts & WAKE_MAGIC) {
			err = jl1xxx_wol_dynamic_op_set(phydev);
			if (err < 0)
				return err;
		}
	}

	return 0;
}
#endif

static int jl1xxx_suspend(struct phy_device *phydev)
{
	return genphy_suspend(phydev);
}

static int jl1xxx_resume(struct phy_device *phydev)
{
	return genphy_resume(phydev);
}

static int jl2xxx_probe(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl2xxx_priv *jl2xxx = NULL;
	int err;

	jl2xxx = devm_kzalloc(dev, sizeof(*jl2xxx), GFP_KERNEL);
	if (!jl2xxx)
		return -ENOMEM;

	phydev->priv = jl2xxx;

#if (JLSEMI_KERNEL_DEVICE_TREE_USE)
	if (!dev->of_node)
		JLSEMI_PHY_MSG("%s: Find device node failed\n", __func__);
#endif
	err = jl2xxx_operation_args_get(phydev);
	if (err < 0)
		return err;

	if (jl2xxx->intr.enable & JL2XXX_INTR_STATIC_OP_EN)
		phydev->irq = JL2XXX_INTR_IRQ;

	jl2xxx->static_inited = false;
	jl2xxx->nstats = ARRAY_SIZE(jl2xxx_hw_stats);
	jl2xxx->hw_stats = jl2xxx_hw_stats;
	jl2xxx->stats = kcalloc(jl2xxx->nstats, sizeof(u64), GFP_KERNEL);
	if (!jl2xxx->stats)
		return -ENOMEM;

	return 0;
}

static int jl2xxx_config_init(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int ret;

	if (!priv->static_inited) {
#if (JLSEMI_DEBUG_INFO)
		JLSEMI_PHY_MSG("jl2xxx_config_init_before:\n");
		ret = jlsemi_phy_reg_print(phydev);
		if (ret < 0)
			return ret;
#endif
		ret = jl2xxx_static_op_init(phydev);
		if (ret < 0)
			return ret;
#if (JLSEMI_DEBUG_INFO)
		JLSEMI_PHY_MSG("jl2xxx_config_init_after:\n");
		ret = jlsemi_phy_reg_print(phydev);
		if (ret < 0)
			return ret;
#endif
		priv->static_inited = JLSEMI_PHY_NOT_REENTRANT;
	}

	return 0;
}

static int jl2xxx_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = jl2xxx_intr_ack_event(phydev);
	if (err < 0)
		return err;

	return 0;
}

static int jl2xxx_config_intr(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->intr.enable & JL2XXX_INTR_STATIC_OP_EN) {
		err = jl2xxx_ack_interrupt(phydev);
		if (err < 0)
			return err;

		err = jl2xxx_intr_static_op_set(phydev);
		if (err < 0)
			return err;
	}

	return 0;
}

static int jl2xxx_read_status(struct phy_device *phydev)
{
	struct jl2xxx_priv *priv = phydev->priv;
	bool fiber_mode;
	int err;

	if (priv->intr.enable & JL2XXX_INTR_STATIC_OP_EN) {
		err = jl2xxx_ack_interrupt(phydev);
		if (err < 0)
			return err;
	}

	fiber_mode = jl2xxx_read_fiber_status(phydev);
	if (fiber_mode)
		return 0;

	return genphy_read_status(phydev);
}

static int jl1xxx_config_aneg(struct phy_device *phydev)
{
	return genphy_config_aneg(phydev);
}

static int jl2xxx_config_aneg(struct phy_device *phydev)
{
	u16 phy_mode;
	int val;

	val = jlsemi_read_paged(phydev, JL2XXX_PAGE18,
				JL2XXX_WORK_MODE_REG);
	phy_mode = val & JL2XXX_WORK_MODE_MASK;

	if (phydev->interface == PHY_INTERFACE_MODE_SGMII)
		return 0;

	if ((phydev->interface != PHY_INTERFACE_MODE_SGMII) &&
	    ((phy_mode == JL2XXX_FIBER_RGMII_MODE) ||
	    (phy_mode == JL2XXX_UTP_FIBER_RGMII_MODE)))
		return jl2xxx_config_aneg_fiber(phydev);

	return genphy_config_aneg(phydev);
}

static int jl2xxx_suspend(struct phy_device *phydev)
{
	return genphy_suspend(phydev);
}

static int jl2xxx_resume(struct phy_device *phydev)
{
	return genphy_resume(phydev);
}

#if (JLSEMI_PHY_WOL)
static void jl2xxx_get_wol(struct phy_device *phydev,
			   struct ethtool_wolinfo *wol)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int wol_en;

	if (priv->wol.ethtool) {
		wol->supported = WAKE_MAGIC;
		wol->wolopts = 0;

		wol_en = jl2xxx_wol_dynamic_op_get(phydev);

		if (wol_en)
			wol->wolopts |= WAKE_MAGIC;
	}
}

static int jl2xxx_set_wol(struct phy_device *phydev,
			  struct ethtool_wolinfo *wol)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int err;

	if (priv->wol.ethtool) {
		if (wol->wolopts & WAKE_MAGIC) {
			err = jl2xxx_wol_dynamic_op_set(phydev);
			if (err < 0)
				return err;
		}
	}

	return 0;
}
#endif

#if (JL2XXX_PHY_TUNABLE)
static int jl2xxx_get_tunable(struct phy_device *phydev,
			      struct ethtool_tunable *tuna, void *data)
{
	struct jl2xxx_priv *priv = phydev->priv;

	switch (tuna->id) {
	case ETHTOOL_PHY_FAST_LINK_DOWN:
		if (priv->fld.ethtool)
			return jl2xxx_fld_dynamic_op_get(phydev, data);
		else
			return 0;
	case ETHTOOL_PHY_DOWNSHIFT:
		if (priv->downshift.ethtool)
			return jl2xxx_downshift_dynamic_op_get(phydev, data);
		else
			return 0;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static int jl2xxx_set_tunable(struct phy_device *phydev,
			      struct ethtool_tunable *tuna, const void *data)
{
	struct jl2xxx_priv *priv = phydev->priv;

	switch (tuna->id) {
	case ETHTOOL_PHY_FAST_LINK_DOWN:
		if (priv->fld.ethtool)
			return jl2xxx_fld_dynamic_op_set(phydev, data);
		else
			return 0;
	case ETHTOOL_PHY_DOWNSHIFT:
		if (priv->downshift.ethtool)
			return jl2xxx_downshift_dynamic_op_set(phydev,
							*(const u8 *)data);
		else
			return 0;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}
#endif

#if (JL2XXX_GET_STAT)
static u64 get_stat(struct phy_device *phydev, int i)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int val;

	val = jlsemi_read_paged(phydev, priv->hw_stats[i].page,
				priv->hw_stats[i].reg);
	if (val < 0)
		return U64_MAX;

	val = val & priv->hw_stats[i].mask;
	priv->stats[i] += val;

	return priv->stats[i];
}

static void jl2xxx_get_stats(struct phy_device *phydev,
			    struct ethtool_stats *stats, u64 *data)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int i;

	if (!priv)
		return;

	for (i = 0; i < priv->nstats; i++)
		data[i] = get_stat(phydev, i);
}
#endif

#if (JL2XXX_GET_STRING)
static void jl2xxx_get_strings(struct phy_device *phydev, u8 *data)
{
	struct jl2xxx_priv *priv = phydev->priv;
	int i;

	if (!priv)
		return;

	for (i = 0; i < priv->nstats; i++)
		strlcpy(data + i * ETH_GSTRING_LEN,
			priv->hw_stats[i].string, ETH_GSTRING_LEN);
}
#endif

static void jl2xxx_remove(struct phy_device *phydev)
{
	struct device *dev = jlsemi_get_mdio(phydev);
	struct jl2xxx_priv *priv = phydev->priv;

	kfree(priv->stats);
	if (priv)
		devm_kfree(dev, priv);
}

static inline int jlsemi_aneg_done(struct phy_device *phydev)
{
	int retval = phy_read(phydev, MII_BMSR);

	return (retval < 0) ? retval : (retval & BMSR_ANEGCOMPLETE);
}

static int jl2xxx_aneg_done(struct phy_device *phydev)
{
	u16 phy_mode;
	int val;

	val = jlsemi_read_paged(phydev, JL2XXX_PAGE18,
				JL2XXX_WORK_MODE_REG);
	phy_mode = val & JL2XXX_WORK_MODE_MASK;

	if (phydev->interface == PHY_INTERFACE_MODE_SGMII)
		return 0;

	// fiber not an complite
	if ((phydev->interface != PHY_INTERFACE_MODE_SGMII) &&
	    ((phy_mode == JL2XXX_FIBER_RGMII_MODE) ||
	    (phy_mode == JL2XXX_UTP_FIBER_RGMII_MODE)))
		return BMSR_ANEGCOMPLETE;

	return jlsemi_aneg_done(phydev);
}


static struct phy_driver jlsemi_drivers[] = {
	{
		.phy_id		= JL1XXX_PHY_ID,
		.phy_id_mask    = JLSEMI_PHY_ID_MASK,
		.name           = DRIVER_NAME_100M,
		/* PHY_BASIC_FEATURES */
		.features	= PHY_BASIC_FEATURES,
		.probe		= jl1xxx_probe,
		.config_intr	= jl1xxx_config_intr,
		.read_status	= jl1xxx_read_status,
		.config_init    = jl1xxx_config_init,
		.config_aneg    = jl1xxx_config_aneg,
		.aneg_done	= jlsemi_aneg_done,
		.suspend        = jl1xxx_suspend,
		.resume         = jl1xxx_resume,
		.remove		= jl1xxx_remove,
#if (JLSEMI_PHY_WOL)
		.get_wol	= jl1xxx_get_wol,
		.set_wol	= jl1xxx_set_wol,
#endif
	},
	{
		.phy_id         = JL2XXX_PHY_ID,
		.phy_id_mask    = JLSEMI_PHY_ID_MASK,
		.name           = DRIVER_NAME_1000M,
		/* PHY_BASIC_FEATURES */
		.features	= PHY_GBIT_FEATURES,
		.probe		= jl2xxx_probe,
		.config_intr	= jl2xxx_config_intr,
		.read_status	= jl2xxx_read_status,
		.config_init    = jl2xxx_config_init,
		.config_aneg    = jl2xxx_config_aneg,
		.aneg_done	= jl2xxx_aneg_done,
		.suspend        = jl2xxx_suspend,
		.resume         = jl2xxx_resume,
		.remove		= jl2xxx_remove,
#if (JLSEMI_PHY_WOL)
		.get_wol	= jl2xxx_get_wol,
		.set_wol	= jl2xxx_set_wol,
#endif
#if (JL2XXX_PHY_TUNABLE)
		.get_tunable	= jl2xxx_get_tunable,
		.set_tunable	= jl2xxx_set_tunable,
#endif
#if (JL2XXX_GET_STAT)
		.get_stats	= jl2xxx_get_stats,
#endif
#if (JL2XXX_GET_STRING)
		.get_strings	= jl2xxx_get_strings,
#endif
	},
};

module_jlsemi_driver(jlsemi_drivers);

static struct mdio_device_id __maybe_unused jlsemi_tbl[] = {
	{JL1XXX_PHY_ID, JLSEMI_PHY_ID_MASK},
	{JL2XXX_PHY_ID, JLSEMI_PHY_ID_MASK},
	{ }
};

MODULE_DEVICE_TABLE(mdio, jlsemi_tbl);
