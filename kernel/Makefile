
include ../obj.mk

ARCHSRC= $(wildcard arch/$(ARCH)/*.c) $(wildcard arch/$(ARCH)/*.s)
ARCHOBJ= $(patsubst %.s,%.s.o, $(patsubst %.c,%.c.o,$(ARCHSRC)))

OBJS= $(ARCHOBJ) main.c.o device/tty/ttys/vgatty.c.o tar.c.o kdebug.c.o mem.c.o \
      kmalloc.c.o syscall.c.o elf.c.o proc.c.o sync.c.o fs.c.o device/initrd.c.o \
	  fs/tarfs.c.o fs/devfs.c.o pipe.c.o device/tty/ansiseq.c.o \
	  device/tty/ttylib.c.o device/tty/ttys/comtty.c.o device/fb.c.o \
	  device/tty/ttys/fbtty.c.o device/tty/kbd.c.o fs/tmpfs.c.o \
	  container/list.c.o container/hashmap.c.o

$(ROOT)/boot/kernel: $(OBJS) kernel.ld ../libc/libk.a
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -T kernel.ld -o $@ $(OBJS) ../libc/libk.a -lgcc

clean:
	rm $(OBJS)
