include ../obj.mk

OBJS=string/strcmp.o string/strlen.o

libc.a: $(OBJS)
	$(AR) cr $@ $?

clean:
	rm libc.a
	rm $(OBJS)
