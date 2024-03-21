#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2024.01
PKG_RELEASE:=1
PKG_HASH:=b99611f1ed237bf3541bdc8434b68c96a6e05967061f992443cb30aabebef5b3

PKG_MAINTAINER:=Tobias Maedel <openwrt@tbspace.de>

UBOOT_USE_BINMAN:=1
UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include ../arm-trusted-firmware-rockchip/atf-version.mk

define U-Boot/Default
  BUILD_TARGET:=rockchip
  UENV:=default
  HIDDEN:=1
endef


# RK3328 boards

define U-Boot/Default/rk3328
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3328
  ATF:=$(RK3328_ATF)
endef

define U-Boot/nanopi-r2c-rk3328
  $(U-Boot/Default/rk3328)
  NAME:=NanoPi R2C
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r2c
endef

define U-Boot/nanopi-r2s-rk3328
  $(U-Boot/Default/rk3328)
  NAME:=NanoPi R2S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r2s
endef

define U-Boot/doornet1-rk3328
  $(U-Boot/Default/rk3328)
  NAME:=DoorNet1
  BUILD_DEVICES:= \
    embedfire_doornet1
endef

# RK3399 boards

define U-Boot/Default/rk3399
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3399
  ATF:=$(RK3399_ATF)
endef

define U-Boot/nanopi-r4s-rk3399
  $(U-Boot/Default/rk3399)
  NAME:=NanoPi R4S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r4s \
    friendlyarm_nanopi-r4s-enterprise
endef

define U-Boot/nanopi-r4se-rk3399
  $(U-Boot/Default/rk3399)
  NAME:=NanoPi R4SE
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r4se
endef

define U-Boot/doornet2-rk3399
  $(U-Boot/Default/rk3399)
  NAME:=DoorNet2
  BUILD_DEVICES:= \
    embedfire_doornet2
endef

# RK3566 boards

define U-Boot/Default/rk3566
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3566
  ATF:=$(RK3566_ATF)
  TPL:=$(RK3566_TPL)
endef

define U-Boot/lubancat-1-rk3566
  $(U-Boot/Default/rk3566)
  NAME:=LubanCan-1N
  BUILD_DEVICES:= \
    embedfire_lubancat-1
endef

define U-Boot/lubancat-1n-rk3566
  $(U-Boot/Default/rk3566)
  NAME:=LubanCan-1N
  BUILD_DEVICES:= \
    embedfire_lubancat-1n
endef

# RK3568 boards

define U-Boot/Default/rk3568
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3568
  ATF:=$(RK3568_ATF)
  TPL:=$(RK3568_TPL)
endef

define U-Boot/nanopi-r5c-rk3568
  $(U-Boot/Default/rk3568)
  NAME:=NanoPi R5C
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r5c
endef

define U-Boot/nanopi-r5s-rk3568
  $(U-Boot/Default/rk3568)
  NAME:=NanoPi R5S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r5s
endef

define U-Boot/lubancat-2-rk3568
  $(U-Boot/Default/rk3568)
  NAME:=LubanCat-2
  BUILD_DEVICES:= \
    embedfire_lubancat-2
endef

define U-Boot/lubancat-2n-rk3568
  $(U-Boot/Default/rk3568)
  NAME:=LubanCat-2N
  BUILD_DEVICES:= \
    embedfire_lubancat-2n
endef

define U-Boot/opc-h66k-rk3568
  $(U-Boot/Default/rk3568)
  NAME:=OPC-H66K
  BUILD_DEVICES:= \
    hinlink_opc-h66k
endef

define U-Boot/opc-h68k-rk3568
  $(U-Boot/Default/rk3568)
  NAME:=OPC-H68K
  BUILD_DEVICES:= \
    hinlink_opc-h68k
endef

define U-Boot/opc-h69k-rk3568
  $(U-Boot/Default/rk3568)
  NAME:=OPC-H69K
  BUILD_DEVICES:= \
    hinlink_opc-h69k
endef

# RK3588 boards

define U-Boot/Default/rk3588
  BUILD_SUBTARGET:=armv8
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3588
  ATF:=$(RK3588_ATF)
  TPL:=$(RK3588_TPL)
endef

define U-Boot/nanopc-t6-rk3588
  $(U-Boot/Default/rk3588)
  NAME:=NanoPC T6
  BUILD_DEVICES:= \
    friendlyarm_nanopc-t6
endef

define U-Boot/lubancat-5-rk3588
  $(U-Boot/Default/rk3588)
  NAME:=LubanCat-5
  BUILD_DEVICES:= \
    embedfire_lubancat-5
endef

define U-Boot/h88k-rk3588
  $(U-Boot/Default/rk3588)
  NAME:=HINLINK H88K
  BUILD_DEVICES:= \
    hinlink_h88k
endef

# RK3588S boards

define U-Boot/nanopi-r6c-rk3588s
  $(U-Boot/Default/rk3588)
  NAME:=NanoPi R6C
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r6c
endef

define U-Boot/nanopi-r6s-rk3588s
  $(U-Boot/Default/rk3588)
  NAME:=NanoPi R6S
  BUILD_DEVICES:= \
    friendlyarm_nanopi-r6s
endef

define U-Boot/lubancat-4-rk3588s
  $(U-Boot/Default/rk3588)
  NAME:=LubanCat-4
  BUILD_DEVICES:= \
    embedfire_lubancat-4
endef

UBOOT_TARGETS := \
  nanopi-r2c-rk3328 \
  nanopi-r2s-rk3328 \
  doornet1-rk3328 \
  nanopi-r4s-rk3399 \
  nanopi-r4se-rk3399 \
  doornet2-rk3399 \
  nanopi-r5s-rk3568 \
  nanopi-r5c-rk3568 \
  nanopc-t6-rk3588 \
  nanopi-r6s-rk3588s \
  nanopi-r6c-rk3588s \
  opc-h66k-rk3568 \
  opc-h68k-rk3568 \
  opc-h69k-rk3568 \
  lubancat-1-rk3566 \
  lubancat-2-rk3568 \
  lubancat-1n-rk3566 \
  lubancat-2n-rk3568 \
  lubancat-4-rk3588s \
  lubancat-5-rk3588 \
  h88k-rk3588

UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes

UBOOT_CUSTOMIZE_CONFIG := \
	--disable SPL_FIT_SIGNATURE \
	--disable TOOLS_MKEFICAPSULE \
	--set-str MKIMAGE_DTC_PATH $(PKG_BUILD_DIR)/scripts/dtc/dtc

UBOOT_MAKE_FLAGS += \
  PATH=$(STAGING_DIR_HOST)/bin:$(PATH) \
  BL31=$(STAGING_DIR_IMAGE)/$(ATF) \
  $(if $(TPL),ROCKCHIP_TPL=$(STAGING_DIR_IMAGE)/$(TPL))

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
ifneq ($(TPL),)
	$(CP) $(PKG_BUILD_DIR)/idbloader.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-idbloader.img
	$(CP) $(PKG_BUILD_DIR)/u-boot.itb $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.itb
else
	$(STAGING_DIR_IMAGE)/loaderimage --pack --uboot $(PKG_BUILD_DIR)/u-boot-dtb.bin $(PKG_BUILD_DIR)/uboot.img 0x200000
	$(CP) $(PKG_BUILD_DIR)/uboot.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-uboot.img
endif
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
