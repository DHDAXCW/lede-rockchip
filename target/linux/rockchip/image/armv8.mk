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

define Device/embedfire_lubancat1
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := LubanCat 1
  SOC := rk3566
  UBOOT_DEVICE_NAME := lubancat1-rk3566
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8125
endef
#TARGET_DEVICES += embedfire_lubancat1

define Device/embedfire_lubancat1n
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := LubanCat1N
  SOC := rk3566
  UBOOT_DEVICE_NAME := lubancat1n-rk3566
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8169 -urngd kmod-ata-ahci
endef
#TARGET_DEVICES += embedfire_lubancat1n

define Device/embedfire_lubancat2
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := LubanCat2
  SOC := rk3568
  UBOOT_DEVICE_NAME := lubancat2-rk3568
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core kmod-r8169
endef
#TARGET_DEVICES += embedfire_lubancat2

define Device/embedfire_lubancat2n
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := LubanCat2N
  SOC := rk3568
  UBOOT_DEVICE_NAME := lubancat2n-rk3568
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8125 kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core
endef
#TARGET_DEVICES += embedfire_lubancat2n

define Device/hinlink_opc-h68k
  DEVICE_VENDOR := HINLINK
  DEVICE_MODEL := OPC-H68K
  SOC := rk3568
  UBOOT_DEVICE_NAME := opc-h68k-rk3568
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8125
endef
#TARGET_DEVICES += hinlink_opc-h68k

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

define Device/friendlyarm_nanopi-r5s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R5S
  SOC := rk3568
  UBOOT_DEVICE_NAME := nanopi-r5s-rk3568
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8125 kmod-nvme kmod-scsi-core
endef
#TARGET_DEVICES += friendlyarm_nanopi-r5s

define Device/friendlyarm_nanopi-r5c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R5C
  SOC := rk3568
  UBOOT_DEVICE_NAME := nanopi-r5c-rk3568
  IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-r8125
endef
#TARGET_DEVICES += friendlyarm_nanopi-r5c

define Device/firefly_station-p2
  DEVICE_VENDOR := Firefly
  DEVICE_MODEL := Station P2
  DEVICE_DTS := rockchip/rk3568-roc-pc
  UBOOT_DEVICE_NAME := station-p2-rk3568
  IMAGE/sysupgrade.img.gz := boot-common | boot-script-bin | rockchip-gpt-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-ikconfig kmod-ata-ahci-platform kmod-r8169
endef
#TARGET_DEVICES += firefly_station-p2
