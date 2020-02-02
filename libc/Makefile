include ../obj.mk

OBJS=string/strcmp.o

libc.a: $(OBJS)
	$(AR) cr $@ $?

clean:
	rm libc.a
	rm $(OBJS)
