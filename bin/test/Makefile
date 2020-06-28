include ../../obj.mk

OBJS = pipe cloexec ansi termios getopt

PROGS = $(patsubst %, $(ROOT)/bin/test/%, $(OBJS))

all: dir $(PROGS)

.PHONY: all clean dir

dir:
	mkdir -p $(ROOT)/bin/test

$(ROOT)/bin/test/%: %.c ../user.ld ../../libc/libc.a
	$(CC) $(CFLAGS) -T ../user.ld -o $@ $<  ../../libc/libc.a -lgcc

clean:
	rm -rf $(PROGS)
