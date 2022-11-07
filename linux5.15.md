# 切换5.15来编译rockchip rk356x设备
### 第一方法：
1. 将源码下载 `git clone https://github.com/DHDAXCW/lede-rockchip`
2. 执行命令来切换6.0内核`sed -i 's/5.4/5.15/g' ./target/linux/rockchip/Makefile`
3. 删掉文件 
```
rm -rf package/kernel/rtl8821cu
rm -rf target/linux/rockchip/files/arch/arm64/boot/dts/rockchip/rk356x.dtsi
rm -rf target/linux/rockchip/files/arch/arm64/boot/dts/rockchip/rk3568.dtsi
```
4. 然后参考下载源代码，更新 feeds 并选择配置等等，即可编译你需要的固件了

### 第二方法：
1. 从5.4编译产生后，切换5.15需要清理编译留下的文件:
```bash
rm -rf ./tmp && rm -rf .config
make clean
make menuconfig
make -j$(($(nproc) + 1)) V=s
```
4. 然后参考下载源代码，更新 feeds 并选择配置等等，即可编译你需要的固件了
