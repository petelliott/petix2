#ifndef TTY_H
#define TTY_H

#include <stddef.h>
#include <sys/types.h>

void tty_init(void);

ssize_t tty_write(const void *buf, size_t count);

ssize_t tty_read(void *buf, size_t count);

#endif
