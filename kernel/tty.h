#ifndef TTY_H
#define TTY_H

#include <stddef.h>

void tty_init(void);

size_t tty_write(const void *buf, size_t count);

size_t tty_read(void *buf, size_t count);

#endif
