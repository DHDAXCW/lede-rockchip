# 切换6.0来编译rockchip rk356x设备
1. 将源码下载 `git clone https://github.com/DHDAXCW/lede-rockchip`
2. 执行命令来切换6.0内核`sed -i 's/5.4/6.0/g' ./target/linux/rockchip/Makefile`
3. 删掉文件，拉取上游的 `rm -rf package/kernel/mac80211 && rm -rf package/kernel/rtl8821cu`
4. 下载上游mac80211 `svn export https://github.com/coolsnowwolf/lede/trunk/package/kernel/mac80211 package/kernel/mac80211`
5. 然后参考下载源代码，更新 feeds 并选择配置等等，即可编译你需要的固件了
