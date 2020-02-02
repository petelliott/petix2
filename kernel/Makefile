
include ../obj.mk

OBJS=arch/$(ARCH)/boot.o main.o tty.o tar.o kdebug.o

$(ROOT)/boot/kernel: $(OBJS) kernel.ld ../libc/libc.a
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -T kernel.ld -o $@ $(OBJS) ../libc/libc.a -lgcc

clean:
	rm $(OBJS)
