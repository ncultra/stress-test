#!/usr/bin/bash

git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
cd linux.git
make defconfig
make
rm -fvr linux.git

