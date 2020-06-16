include ../../obj.mk

OBJS=main.c.o

$(ROOT)/bin/sh: $(OBJS) ../user.ld ../../libc/libc.a
	$(CC) $(CFLAGS) -T ../user.ld -o $@ $(OBJS)  ../../libc/libc.a -lgcc

clean:
	rm $(OBJS)
