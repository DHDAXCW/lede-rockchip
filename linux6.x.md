# 切换6.1来编译rockchip rk356x设备
1. 执行命令来切换6.1内核`sed -i 's/5.4/6.1/g' ./target/linux/rockchip/Makefile`
2. 删除源码部分文件夹
```bash
rm -rf package/kernel/mac80211 
rm -rf package/kernel/rtl8821cu
rm -rf package/network/services/hostapd
```
3.下载openwrt官网部分文件
```bash
svn export https://github.com/DHDAXCW/openwrt-beta/trunk/package/kernel/mac80211 package/kernel/mac80211
svn export https://github.com/DHDAXCW/openwrt-beta/trunk/package/network/services/hostapd package/network/services/hostapd
```
4. 将在target/linux/rockchip/image/armv8.mk这个文件夹里，去掉注释#，如下：
```patch
--- a/target/linux/rockchip/image/armv8.mk
+++ b/target/linux/rockchip/image/armv8.mk
@@ -30,7 +30,7 @@ define Device/embedfire_lubancat1
   IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
   DEVICE_PACKAGES := kmod-r8125
 endef
-#TARGET_DEVICES += embedfire_lubancat1
+TARGET_DEVICES += embedfire_lubancat1
 
 define Device/embedfire_lubancat1n
   DEVICE_VENDOR := EmbedFire
@@ -40,7 +40,7 @@ define Device/embedfire_lubancat1n
   IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
   DEVICE_PACKAGES := kmod-r8169 -urngd kmod-ata-ahci
 endef
-#TARGET_DEVICES += embedfire_lubancat1n
+TARGET_DEVICES += embedfire_lubancat1n
 
 define Device/embedfire_lubancat2
   DEVICE_VENDOR := EmbedFire
@@ -50,7 +50,7 @@ define Device/embedfire_lubancat2
   IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
   DEVICE_PACKAGES := kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core
 endef
-#TARGET_DEVICES += embedfire_lubancat2
+TARGET_DEVICES += embedfire_lubancat2
 
 define Device/embedfire_lubancat2n
   DEVICE_VENDOR := EmbedFire
@@ -60,7 +60,7 @@ define Device/embedfire_lubancat2n
   IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
   DEVICE_PACKAGES := kmod-r8125 kmod-ata-ahci kmod-ata-ahci-platform kmod-ata-core
 endef
-#TARGET_DEVICES += embedfire_lubancat2n
+TARGET_DEVICES += embedfire_lubancat2n
 
 define Device/hinlink_opc-h68k
   DEVICE_VENDOR := HINLINK
@@ -70,7 +70,7 @@ define Device/hinlink_opc-h68k
   IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
   DEVICE_PACKAGES := kmod-r8125
 endef
-#TARGET_DEVICES += hinlink_opc-h68k
+TARGET_DEVICES += hinlink_opc-h68k
 
 define Device/friendlyarm_nanopi-r2c
   DEVICE_VENDOR := FriendlyARM
@@ -120,4 +120,4 @@ define Device/friendlyarm_nanopi-r5s
   IMAGE/sysupgrade.img.gz := boot-common | boot-script nanopi-r5s | pine64-img | gzip | append-metadata
   DEVICE_PACKAGES := kmod-r8125 kmod-nvme kmod-scsi-core
 endef
-#TARGET_DEVICES += friendlyarm_nanopi-r5s
+TARGET_DEVICES += friendlyarm_nanopi-r5s
```

5. 然后参考下载源代码，更新 feeds 并选择配置等等，即可编译你需要的固件了
