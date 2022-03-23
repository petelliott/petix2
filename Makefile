CC=i686-elf-gcc
CFLAGS=-Wall -I$(shell pwd)/include/ -nostdlib -ffreestanding
LIBS=-lgcc
ROOT=$(shell pwd)/buildroot
ARCH=i686
export

.PHONY: debug clean petix2.iso run run-iso release

debug: CFLAGS+=-g3 -ggdb -O0
debug: subdir

release: CFLAGS+=-O2 -s
release: subdir

petix2.iso:
	grub-mkrescue -o $@ $(ROOT)

run:
	qemu-system-i386 -initrd "$(ROOT)/boot/initrd.tar initrd" \
	                 -kernel $(ROOT)/boot/kernel \
	                 -serial mon:stdio

run-iso:
	qemu-system-i386 -cdrom petix2.iso \
	                 -serial mon:stdio


gdb:
	qemu-system-i386 -initrd "$(ROOT)/boot/initrd.tar initrd" \
	                 -kernel $(ROOT)/boot/kernel \
	                 -S -s &
	sleep .4
	gdb -x .gdbinit

clean: subdir_clean
	rm -r $(ROOT)
	rm petix2.iso || true


DIRS=skel libc kernel bin initrd

include subdirs.mk
