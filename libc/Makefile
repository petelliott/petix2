include ../obj.mk

OBJS=string/strcmp.o string/strlen.o string/memset.o string/memcpy.o

libc.a: $(OBJS)
	$(AR) cr $@ $?

clean:
	rm libc.a
	rm $(OBJS)
