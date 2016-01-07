#!/usr/bin/bash

git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
wait
cd linux.git
echo "building linux kernel"
make defconfig
wait
make
rm -fvr linux.git
cd ..
