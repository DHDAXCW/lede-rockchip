## The source code is suitable for Rockchip OpenWrt SDK
### Only supported devices:
```
embedfire_doornet1
embedfire_doornet2
embedfire_lubancat1
embedfire_lubancat1n
embedfire_lubancat2
embedfire_lubancat2n
friendlyarm_nanopi-r2c
friendlyarm_nanopi-r2s
friendlyarm_nanopi-r4s
friendlyarm_nanopi-r4se
friendlyarm_nanopi-r5s
friendlyarm_nanopi-r5c
friendlyarm_nanopi-r6c
friendlyarm_nanopi-r6s
friendlyarm_nanopc-t6
hinlink_opc-h66k
hinlink_opc-h68k
hinlink_opc-h69k
```

### Next plan to add equipment:
```
hinlink_h88k
```
How to compile the OpenWrt firmware you need
-
Notice:
-
1. **NOT** compile with **root** user! ! !
2. Domestic users had better prepare a ladder before compiling
3. Default login IP 192.168.1.1 password password

2. Install compilation dependencies

   ```bash
   sudo apt update -y
   sudo apt install -y ack antlr3 asciidoc autoconf automake autopoint binutils bison build-essential \
   bzip2 ccache cmake cpio curl device-tree-compiler fastjar flex gawk gettext gcc-multilib g++-multilib \
   git gperf haveged help2man intltool libc6-dev-i386 libelf-dev libfuse-dev libglib2.0-dev libgmp3-dev \
   libltdl-dev libmpc-dev libmpfr-dev libncurses5-dev libncursesw5-dev libpython3-dev libreadline-dev \
   libssl-dev libtool lrzsz mkisofs msmtp ninja-build p7zip p7zip-full patch pkgconf python2.7 python3 \
   python3-pyelftools python3-setuptools qemu-utils rsync scons squashfs-tools subversion swig texinfo \
   uglifyjs upx-ucl unzip vim wget xmlto xxd zlib1g-dev
   ```

3. Download source code, update feeds and choose configuration

   ```bash
   git clone https://github.com/DHDAXCW/lede-rockchip
   cd lede-rockchip
   ./scripts/feeds update -a
   ./scripts/feeds install -a
   make menuconfig
   ```

5. `make -j8 download V=s` download dl library

6. Enter `make -j10 V=s` (-j1 is followed by the number of threads. It is recommended to use single thread for the first compilation) to start compiling the firmware you want.

This set of code is guaranteed to compile successfully. It includes all source codes of R24, including IPK.

Second compilation:
```bash
cd lede
git pull
./scripts/feeds update -a && ./scripts/feeds install -a
make defconfig
make -j8 download
make -j$(($(nproc) + 1)) V=s
```

If reconfiguration is required:
```bash
rm -rf ./tmp && rm -rf .config
make menuconfig
make -j$(($(nproc) + 1)) V=s
```

- Output path after compilation is complete：bin/targets

- from upstream source code https://github.com/coolsnowwolf/lede
