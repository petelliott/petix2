CC=i686-elf-gcc
CFLAGS=-Wall -g -I$(shell pwd)/include/ -nostdlib -O2 -ffreestanding
LIBS=-lgcc
ROOT=$(shell pwd)/buildroot
ARCH=i686
export

.PHONY: all clean petix2.iso run

all: subdir

petix2.iso:
	grub-mkrescue -o $@ $(ROOT)

run:
	qemu-system-i386 -initrd "$(ROOT)/boot/initrd.tar initrd" \
	                 -kernel $(ROOT)/boot/kernel

gdb:
	qemu-system-i386 -initrd "$(ROOT)/boot/initrd.tar initrd" \
	                 -kernel $(ROOT)/boot/kernel \
	                 -S -s &
	sleep 1
	gdb -x .gdbinit

clean: subdir_clean
	rm -r $(ROOT)
	rm petix2.iso || true

DIRS=skel libc kernel bin initrd

include subdirs.mk
