
include ../obj.mk

ARCHSRC= $(wildcard arch/$(ARCH)/*.c) $(wildcard arch/$(ARCH)/*.s)
ARCHOBJ= $(patsubst %.s,%.o, $(patsubst %.c,%.o,$(ARCHSRC)))

OBJS= $(ARCHOBJ) main.o tty.o tar.o kdebug.o initramfs.o mem.o \
	kmalloc.o

$(ROOT)/boot/kernel: $(OBJS) kernel.ld ../libc/libc.a
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -T kernel.ld -o $@ $(OBJS) ../libc/libc.a -lgcc

clean:
	rm $(OBJS)
