#!/bin/bash

./iso.sh && qemu-system-i386 -display gtk,zoom-to-fit=on -cdrom oruos.iso -serial file:serial.log && ./clean.sh
