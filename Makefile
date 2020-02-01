CC=i686-elf-gcc
CFLAGS=-Wall -g -I$(shell pwd)/include/ -nostdlib -O2
LIBS=-lgcc
ROOT=$(shell pwd)/buildroot/
ARCH=i686
export

.PHONY: all clean petix2.iso run

all: subdir petix2.iso

petix2.iso:
	grub-mkrescue -o $@ $(ROOT)

run:
	qemu-system-i386 -cdrom petix2.iso

clean: subdir_clean
	rm -r $(ROOT)

DIRS=skel kernel

include subdirs.mk
