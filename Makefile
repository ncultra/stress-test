
.PHONY tar:
tar:
	$(shell tar czf ../stress-test.tar.gz .  --overwrite --exclude='.git/*' --exclude='*.tar.gz') 

#KDIR=/home/mdday/rpmbuild/BUILD/kernel-4.1.fc21/linux-4.1.10-100.fc21.x86_64
KDIR ?= /lib/modules/`uname -r`/build
ifneq ($(KERNELRELEASE),)
obj-m := stress-alloc.o
endif

.PHONY stress-alloc:
stress-alloc:
	$(MAKE) -C $(KDIR) M=$$PWD
