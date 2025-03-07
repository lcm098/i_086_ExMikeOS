#!/bin/sh

# This script starts the QEMU PC emulator, booting from the
# MikeOS floppy disk image

qemu-system-x86_64 -fda disk_images/mikeos.flp -display gtk,zoom-to-fit=on

