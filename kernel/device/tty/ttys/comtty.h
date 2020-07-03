#ifndef TTY_COMTTY_H
#define TTY_COMTTY_H

#include <stddef.h>
#include <sys/types.h>

void comtty_init(void);

ssize_t comtty_write(const void *buf, size_t count);

#endif
