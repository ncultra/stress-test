
.PHONY tar:
tar:
	$(shell tar czf ../stress-test.tar.gz .  --overwrite --exclude='.git/*' --exclude='*.tar.gz') 

