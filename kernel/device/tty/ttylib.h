#ifndef DEVICE_TTY_TTYLIB_H
#define DEVICE_TTY_TTYLIB_H

#include <termios.h>
#include <stddef.h>
#include "../../sync.h"
#include "ansiseq.h"
#include <stdarg.h>

#define TTY_BUFF_LEN 2048

struct petix_tty {
    struct termios termios;
    struct ansi_term output;
    char buffer[TTY_BUFF_LEN];
    size_t fbase, lbase, loff;
    petix_lock_t read_lock, write_lock;
    petix_sem_t read_sem;
};

void petix_tty_init(struct petix_tty *tty, const struct ansi_backend *out);

ssize_t petix_tty_read(struct petix_tty *tty, char *buf, size_t count);
ssize_t petix_tty_write(struct petix_tty *tty, const char *buf, size_t count);

void petix_tty_input_seq(struct petix_tty *tty, const char *seq, size_t n);

ssize_t petix_tty_ioctl(struct petix_tty *tty, size_t req, va_list ap);

#endif
