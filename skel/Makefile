CONFS=boot/grub/grub.cfg etc/motd

ROOTCONFS=$(addprefix $(ROOT)/,$(CONFS))


all: $(ROOTCONFS)

$(ROOT)/%: %
	mkdir -p $(dir $@)
	cp $^ $@

clean:

.PHONY: all clean
