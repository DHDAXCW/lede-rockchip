# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Tobias Maedel

define Device/embedfire_doornet1
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := DoorNet1
  SOC := rk3328
  UBOOT_DEVICE_NAME := doornet1-rk3328
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r2s | pine64-bin | gzip | append-metadata
  DEVICE_PACKAGES := kmod-usb-net-rtl8152 kmod-rtl8821cu
endef
TARGET_DEVICES += embedfire_doornet1

define Device/embedfire_doornet2
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := DoorNet2
  SOC := rk3399
  UBOOT_DEVICE_NAME := doornet2-rk3399
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r4s | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8168 -urngd
endef
TARGET_DEVICES += embedfire_doornet2

define Device/friendlyarm_nanopi-neo3
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi NEO3
  SOC := rk3328
  UBOOT_DEVICE_NAME := nanopi-r2s-rk3328
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r2s | pine64-bin | gzip | append-metadata
endef
TARGET_DEVICES += friendlyarm_nanopi-neo3

define Device/friendlyarm_nanopi-r2c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2C
  SOC := rk3328
  UBOOT_DEVICE_NAME := nanopi-r2c-rk3328
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r2s | pine64-bin | gzip | append-metadata
  DEVICE_PACKAGES := kmod-usb-net-rtl8152  kmod-rtl8821cu
endef
TARGET_DEVICES += friendlyarm_nanopi-r2c

define Device/friendlyarm_nanopi-r2s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2S
  SOC := rk3328
  UBOOT_DEVICE_NAME := nanopi-r2s-rk3328
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r2s | pine64-bin | gzip | append-metadata
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2s

define Device/friendlyarm_nanopi-r4s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4S
  SOC := rk3399
  UBOOT_DEVICE_NAME := nanopi-r4s-rk3399
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r4s | pine64-bin | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8168  kmod-rtl8821cu -urngd
endef
TARGET_DEVICES += friendlyarm_nanopi-r4s 

define Device/friendlyarm_nanopi-r4se
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4SE
  SOC := rk3399
  UBOOT_DEVICE_NAME := nanopi-r4se-rk3399
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r4s | pine64-bin | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8168 -urngd
endef
TARGET_DEVICES += friendlyarm_nanopi-r4se
