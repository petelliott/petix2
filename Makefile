CC=i686-elf-gcc
CFLAGS=-Wall -g -I$(shell pwd)/include/ -nostdlib -O2 -ffreestanding
LIBS=-lgcc
ROOT=$(shell pwd)/buildroot
ARCH=i686
export

.PHONY: all clean petix2.iso run

all: subdir

iso:
	grub-mkrescue -o $@ $(ROOT)

run:
	qemu-system-i386 -initrd "$(ROOT)/boot/initrd.tar initrd" \
	                 -kernel $(ROOT)/boot/kernel

clean: subdir_clean
	rm -r $(ROOT)
	rm petix2.iso || true

DIRS=skel libc kernel initrd

include subdirs.mk
