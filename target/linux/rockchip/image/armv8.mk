# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Tobias Maedel

# FIT will be loaded at 0x02080000. Leave 16M for that, align it to 2M and load the kernel after it.
KERNEL_LOADADDR := 0x03200000

define Device/embedfire_doornet1
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := DoorNet1
  SOC := rk3328
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-usb-net-rtl8152 kmod-rtl8821cu
endef
TARGET_DEVICES += embedfire_doornet1

define Device/embedfire_doornet2
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := DoorNet2
  SOC := rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-r8168 -urngd
endef
TARGET_DEVICES += embedfire_doornet2

define Device/embedfire_lubancat-1
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := LubanCat-1
  SOC := rk3566
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += embedfire_lubancat-1

define Device/embedfire_lubancat-1n
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := LubanCat-1N
  SOC := rk3566
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8169 -urngd kmod-ata-ahci
endef
TARGET_DEVICES += embedfire_lubancat-1n

define Device/embedfire_lubancat-2
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := LubanCat-2
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core
endef
TARGET_DEVICES += embedfire_lubancat-2

define Device/embedfire_lubancat-2n
  DEVICE_VENDOR := EmbedFire
  DEVICE_MODEL := LubanCat-2N
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125 kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core
endef
TARGET_DEVICES += embedfire_lubancat-2n

define Device/hinlink_common
  DEVICE_VENDOR := HINLINK
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ata-ahci-platform kmod-hwmon-pwmfan kmod-mt7921e kmod-r8125 wpad-openssl
endef

define Device/hinlink_opc-h66k
$(call Device/hinlink_common)
  DEVICE_MODEL := OPC-H66K
  SOC := rk3568
endef
TARGET_DEVICES += hinlink_opc-h66k

define Device/hinlink_opc-h68k
$(call Device/hinlink_common)
  DEVICE_MODEL := OPC-H68K
  SOC := rk3568
endef
TARGET_DEVICES += hinlink_opc-h68k

define Device/hinlink_opc-h69k
$(call Device/hinlink_common)
  DEVICE_MODEL := OPC-H69K
  SOC := rk3568
  DEVICE_PACKAGES += kmod-usb-serial-option uqmi
endef
TARGET_DEVICES += hinlink_opc-h69k

define Device/hinlink_h88k
  DEVICE_VENDOR := HINLINK
  DEVICE_MODEL := H88K
  SOC := rk3588
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ata-ahci-platform kmod-hwmon-pwmfan kmod-mt7921e kmod-r8125 wpad-openssl
endef
TARGET_DEVICES += hinlink_h88k

define Device/friendlyarm_nanopi-r2c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2C
  SOC := rk3328
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2c

define Device/friendlyarm_nanopi-r2s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R2S
  SOC := rk3328
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += friendlyarm_nanopi-r2s

define Device/friendlyarm_nanopi-r4s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4S
  SOC := rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-r8168  kmod-rtl8821cu -urngd
endef
TARGET_DEVICES += friendlyarm_nanopi-r4s 

define Device/friendlyarm_nanopi-r4se
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R4SE
  SOC := rk3399
  BOOT_FLOW := pine64-bin
  DEVICE_PACKAGES := kmod-r8168 -urngd
endef
TARGET_DEVICES += friendlyarm_nanopi-r4se

define Device/friendlyarm_nanopi-r5s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R5S
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-ata-ahci-platform kmod-hwmon-pwmfan kmod-mt7921e kmod-r8125 wpad-openssl
endef
TARGET_DEVICES += friendlyarm_nanopi-r5s

define Device/friendlyarm_nanopi-r5c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R5C
  SOC := rk3568
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-r8125
endef
TARGET_DEVICES += friendlyarm_nanopi-r5c

define Device/friendlyarm_nanopc-t6
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPC T6
  SOC := rk3588
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-hwmon-pwmfan kmod-mt7921e kmod-r8125 wpad-openssl
endef
TARGET_DEVICES += friendlyarm_nanopc-t6

define Device/friendlyarm_nanopi-r6s
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R6S
  SOC := rk3588s
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-hwmon-pwmfan kmod-mt7921e kmod-r8125 wpad-openssl
endef
TARGET_DEVICES += friendlyarm_nanopi-r6s

define Device/friendlyarm_nanopi-r6c
  DEVICE_VENDOR := FriendlyARM
  DEVICE_MODEL := NanoPi R6C
  SOC := rk3588s
  BOOT_FLOW := pine64-img
  DEVICE_PACKAGES := kmod-hwmon-pwmfan kmod-mt7921e kmod-r8125 wpad-openssl
endef
TARGET_DEVICES += friendlyarm_nanopi-r6c
