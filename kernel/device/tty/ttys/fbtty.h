#ifndef TTY_FBTTY_H
#define TTY_FBTTY_H

#include <stddef.h>
#include <sys/types.h>

void fbtty_init(void);

ssize_t fbtty_write(const void *buf, size_t count);

#endif
