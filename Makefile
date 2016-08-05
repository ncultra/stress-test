KDIR := /lib/modules/`uname -r`/build


.PHONY tar:
tar:
	tar czf stress-test.tar.gz  .  --overwrite --exclude='.git/*' --exclude='*.tar.gz' --exclude='*.tmp' --exclude='*.o' --exclude='*.ko'

.PHONY clean:
clean:
	rm *.o *.a *.ko &>/dev/null

.PHONY stress-alloc:
stress-alloc:
	make -C ${KDIR} M=${PWD} modules

.PHONY stress-clean:
stress-clean:
	make -C ${KDIR} M=${PWD} clean

.PHONY stress-install:
stress-install:
	make -C ${KDIR} M=${PWD} modules_install
