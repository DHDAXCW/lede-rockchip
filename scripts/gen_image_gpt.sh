#!/bin/sh
# Copyright (C) 2006-2012 OpenWrt.org
set -e -x
if [ $# -ne 5 ] && [ $# -ne 6 ]; then
    echo "SYNTAX: $0 <file> <kernel size> <kernel directory> <rootfs size> <rootfs image>"
    exit 1
fi

OUTPUT="$1"
KERNELSIZE="$2"
KERNELDIR="$3"
ROOTFSSIZE="$4"
ROOTFSIMAGE="$5"

UBOOTSIZE=8
UBOOTOFFSET=8
KERNELOFFSET="$(($UBOOTOFFSET + $UBOOTSIZE))"
ROOTFSOFFSET="$(($KERNELSIZE + $KERNELOFFSET))"

rm -f "$OUTPUT"

# create partition table
set $(ptgen -g -o "$OUTPUT" -a 2 -N uboot -p "$UBOOTSIZE"M@"$UBOOTOFFSET"M -N kernel -p "$KERNELSIZE"M@"$KERNELOFFSET"M -N rootfs -p "$ROOTFSSIZE"M@"$ROOTFSOFFSET"M)

KERNELOFFSET="$(($3 / 512))"
KERNELSIZE="$4"
ROOTFSOFFSET="$(($5 / 512))"
ROOTFSSIZE="$(($6 / 512))"

[ -n "$PADDING" ] && dd if=/dev/zero of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc count="$ROOTFSSIZE"
dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc

if [ -n "$GUID" ]; then
    [ -n "$PADDING" ] && dd if=/dev/zero of="$OUTPUT" bs=512 seek="$((ROOTFSOFFSET + ROOTFSSIZE))" conv=notrunc count="$sect"
    mkfs.fat -n kernel -C "$OUTPUT.kernel" -S 512 "$((KERNELSIZE / 1024))"
    mcopy -s -i "$OUTPUT.kernel" "$KERNELDIR"/* ::/
else
    make_ext4fs -J -L kernel -l "$KERNELSIZE" "$OUTPUT.kernel" "$KERNELDIR"
fi
dd if="$OUTPUT.kernel" of="$OUTPUT" bs=512 seek="$KERNELOFFSET" conv=notrunc
rm -f "$OUTPUT.kernel"
