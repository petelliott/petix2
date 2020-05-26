
.PHONY: $(ROOT)/boot/initrd.tar $(ROOT)/boot/initrd.tar.gz clean

all: $(ROOT)/boot/initrd.tar $(ROOT)/boot/initrd.tar.gz

FILES=etc bin

$(ROOT)/boot/initrd.tar:
	tar -b 1 -cvf $@ -C $(ROOT) $(FILES)

$(ROOT)/boot/initrd.tar.gz:
	tar -b 1 -czvf $@ -C $(ROOT) $(FILES)
