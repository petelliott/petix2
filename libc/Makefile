include ../obj.mk

OBJS = string/strcmp.c.o string/strlen.c.o string/memset.c.o \
       string/memcpy.c.o sys/syscall.s.o

libc.a: $(OBJS)
	$(AR) cr $@ $?

clean:
	rm libc.a
	rm $(OBJS)
