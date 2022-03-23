
.PHONY: $(ROOT)/boot/initrd.tar $(ROOT)/boot/initrd.tar.gz clean

all: $(ROOT)/boot/initrd.tar $(ROOT)/boot/initrd.tar.gz

FILES=etc share bin dev man COPYING tmp

$(ROOT)/boot/initrd.tar:
	tar -b 1 -cf $@ -C $(ROOT) $(FILES)

$(ROOT)/boot/initrd.tar.gz:
	tar -b 1 -czf $@ -C $(ROOT) $(FILES)
