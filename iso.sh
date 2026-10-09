#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/oruos.kernel isodir/boot/oruos.kernel
cat >isodir/boot/grub/grub.cfg <<EOF
set default=0
set timeout_style=hidden
set timeout=0
menuentry "oruos" {
	multiboot2 /boot/oruos.kernel
}
EOF
grub-mkrescue -o oruos.iso isodir

#set gfxpayload=keep
