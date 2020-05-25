
include ../obj.mk

ARCHSRC= $(wildcard arch/$(ARCH)/*.c) $(wildcard arch/$(ARCH)/*.s)
ARCHOBJ= $(patsubst %.s,%.s.o, $(patsubst %.c,%.c.o,$(ARCHSRC)))

OBJS= $(ARCHOBJ) main.c.o tty.c.o tar.c.o kdebug.c.o initramfs.c.o mem.c.o \
	kmalloc.c.o syscall.c.o

$(ROOT)/boot/kernel: $(OBJS) kernel.ld ../libc/libc.a
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -T kernel.ld -o $@ $(OBJS) ../libc/libc.a -lgcc

clean:
	rm $(OBJS)
