
.PHONY: $(ROOT)/boot/initrd.tar clean

FILES=etc

$(ROOT)/boot/initrd.tar.gz:
	tar -b 1 -czvf $@ -C $(ROOT) $(FILES)
