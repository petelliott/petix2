CONFS=boot/grub/grub.cfg etc dev man COPYING

ROOTCONFS=$(addprefix $(ROOT)/,$(CONFS))


all: $(ROOTCONFS)

$(ROOT)/%: %
	rm -rf $@
	mkdir -p $(dir $@)
	cp -r $^ $@

clean:

.PHONY: all clean
