CONFS=boot/grub/grub.cfg etc dev COPYING

ROOTCONFS=$(addprefix $(ROOT)/,$(CONFS))


all: $(ROOTCONFS)

$(ROOT)/%: %
	mkdir -p $(dir $@)
	cp -r $^ $@

clean:

.PHONY: all clean
