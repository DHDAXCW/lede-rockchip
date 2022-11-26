#
# Copyright (C) 2012 Jo-Philipp Wich <jo@mein.io>
#
# This is free software, licensed under the Apache 2 license.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=quectel-cm
PKG_RELEASE:=1

PKG_MAINTAINER:=Kyosuke Nekoyashiki <supercatexpert@gmail.com>
PKG_LICENSE:=Public Domain

include $(INCLUDE_DIR)/package.mk

define Package/quectel-cm
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Simple controller for Quectel 5G/LTE modems.
endef

define Package/quectel-cm/description
  A modem controller utiltiy for Quectel 5G/LTE modem module.
endef

define Build/Configure
endef

define Build/Compile
	cd ${PKG_BUILD_DIR}; $(MAKE) CROSS_COMPILE="$(TARGET_CROSS)"
endef

define Package/quectel-cm/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/quectel-cm $(1)/usr/bin/quectel-cm
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/quectel-qmi-proxy $(1)/usr/bin/quectel-qmi-proxy
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/quectel-mbim-proxy $(1)/usr/bin/quectel-mbim-proxy
endef

$(eval $(call BuildPackage,quectel-cm))
