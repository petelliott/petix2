include ../../obj.mk

OBJS = echo cat ls wc more md

PROGS = $(patsubst %, $(ROOT)/bin/%, $(OBJS))

all: $(PROGS)

.PHONY: all clean

$(ROOT)/bin/%: %.c ../user.ld ../../libc/libc.a
	$(CC) $(CFLAGS) -T ../user.ld -o $@ $<  ../../libc/libc.a -lgcc

clean:
	rm -rf $(PROGS)
