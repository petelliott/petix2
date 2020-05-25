include ../obj.mk

OBJS = string/strcmp.c.o string/strlen.c.o string/memset.c.o \
       string/memcpy.c.o sys/syscall.s.o sys/crt0.s.o

KOBJS = string/strcmp.c.o string/strlen.c.o string/memset.c.o \
       string/memcpy.c.o

all: libc.a libk.a
.PHONY: all clean

libc.a: $(OBJS)
	$(AR) cr $@ $?

libk.a: $(OBJS)
	$(AR) cr $@ $?

clean:
	rm libc.a libk.a
	rm $(OBJS)
