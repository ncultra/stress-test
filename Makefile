
.PHONY tar:
tar:
	tar czf stress-test.tar.gz  .  --overwrite --exclude='.git/*' --exclude='*.tar.gz' --exclude='*.tmp' --exclude='*.o' --exclude='*.ko'

.PHONY clean:
clean:
rm *.o *.a *.ko &>/dev/null
