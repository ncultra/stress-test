
.PHONY tar:
tar:
	$(shell tar czf stress-test.tar.gz .  --exclude='.git/*' --exclude='*.tar.gz') 

