include ../../obj.mk

PROGS= $(ROOT)/bin/echo

all: $(PROGS)

.PHONY: all clean

$(ROOT)/bin/%: %.c ../user.ld ../../libc/libc.a
	$(CC) $(CFLAGS) -T ../user.ld -o $@ $<  ../../libc/libc.a -lgcc

clean:
	rm -rf $(PROGS)
