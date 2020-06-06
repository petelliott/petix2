include ../obj.mk

OBJS = string/strcmp.c.o string/strlen.c.o string/memset.c.o \
       string/memcpy.c.o string/memcmp.c.o sys/syscall.s.o \
       sys/crt0.s.o string/strerror.c.o string/strtok.c.o \
       string/strchr.c.o string/strncpy.c.o sys/syscall.c.o \
       errno/errno.c.o fcntl/open.c.o unistd/read.c.o unistd/fork.c.o \
       unistd/write.c.o unistd/close.c.o sys/wait.c.o unistd/exec.c.o \
       unistd/dup2.c.o bits/baseprintf.c.o stdio/file.c.o \
       stdio/fwrite.c.o stdio/fread.c.o stdio/fclose.c.o \
       stdio/fputc.c.o stdio/printf.c.o stdio/fgetc.c.o \
       stdio/fgets.c.o stdio/feof.c.o stdio/fputs.c.o stdio/perror.c.o

KOBJS = string/strcmp.c.o string/strlen.c.o string/memset.c.o \
        string/memcpy.c.o string/memcmp.c.o string/strerror.c.o \
        string/strtok.c.o string/strchr.c.o string/strncpy.c.o \
        bits/baseprintf.c.o

all: libc.a libk.a
.PHONY: all clean

libc.a: $(OBJS)
	$(AR) cr $@ $?

libk.a: $(OBJS)
	$(AR) cr $@ $?

clean:
	rm libc.a libk.a
	rm $(OBJS)
