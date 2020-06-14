
.PHONY: all clean dir

all: dir subdir

dir:
	mkdir -p $(ROOT)/bin

clean: subdir_clean

DIRS=init getty sh coreutils test

include ../subdirs.mk
