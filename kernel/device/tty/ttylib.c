#include "ttylib.h"
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>

#define SEND_CHAR -1

void petix_tty_init(struct petix_tty *tty, const struct ansi_backend *out) {
    memset(tty, 0, sizeof(struct petix_tty));

    tty->output.backend = out;
    ansi_init(&(tty->output));

    tty->termios.c_lflag = ICANON | ECHO | ISIG;
}

#define MIN(a, b) ((a < b)? a:b)

ssize_t petix_tty_read(struct petix_tty *tty, char *buf, size_t count) {
    acquire_lock(&tty->read_lock);

    ssize_t c = count;
    while (count > 0) {
        acquire_global();
        if (tty->lbase > tty->fbase) {
            size_t start = tty->fbase;
            size_t len = MIN(tty->lbase-tty->fbase, count);
            tty->fbase += len;

            memcpy(buf, tty->buffer+start, len);
            count -= len;
            buf += len;

        } else if (tty->lbase < tty->fbase) {
            size_t start1 = tty->fbase;
            size_t len1 = MIN(TTY_BUFF_LEN - tty->fbase, count);
            tty->fbase += len1;
            tty->fbase %= TTY_BUFF_LEN;
            count -= len1;

            size_t start2 = 0;
            size_t len2 = MIN(tty->lbase - 0, count);
            tty->fbase += len2;
            tty->fbase %= TTY_BUFF_LEN;
            count -= len2;

            memcpy(buf, tty->buffer+start1, len1);
            memcpy(buf+len1, tty->buffer+start2, len2);
            buf += len1+len2;
        }
        release_global();

        //eot
        if (buf[-1] == SEND_CHAR) {
            release_lock(&tty->read_lock);
            return (c-count) - 1;
        }

        if (count > 0) {
            cond_wait(&tty->read_sem);
        }
    }

    if (tty->buffer[tty->fbase] == SEND_CHAR) {
        tty->fbase = (tty->fbase + 1) % TTY_BUFF_LEN;
    }

    release_lock(&tty->read_lock);
    return c;
}

ssize_t petix_tty_write(struct petix_tty *tty, const char *buf, size_t count) {
    acquire_lock(&tty->write_lock);


    for (size_t i = 0; i < count; ++i) {
        ansi_putch(&tty->output, ((const char *)buf)[i]);
    }

    release_lock(&tty->write_lock);

    return count;
}

//converts from ascii to caret notation
static const char *const echo_map[128] = {
    "^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "\b", "        ",
    "\n", "^K", "^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S", "^T",
    "^U", "^V", "^W", "^X", "^Y", "^Z", "^[", "^\\", "^]", "^^", "^_",
    " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",",
    "-", ".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    ":", ";", "<", "=", ">", "?", "@", "A", "B", "C", "D", "E", "F",
    "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S",
    "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_", "`",
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "{", "|", "}", "~", "^?"
};

void petix_tty_input_seq(struct petix_tty *tty, const char *seq, size_t n) {
    // we assume this is called while the global lock is acquired
    if (seq[0] == '\b' && n == 1) {
        if (tty->loff != tty->lbase) {
            tty->loff = (((tty->loff - 1)%TTY_BUFF_LEN)+TTY_BUFF_LEN)
                % TTY_BUFF_LEN;

            petix_tty_write(tty, "\b\b\b\b\b\b\b\b\b",
                            strlen(echo_map[(uint8_t)tty->buffer[tty->loff]]));
        }
    } else {

        if (seq[0] != 0x04 || !(tty->termios.c_lflag & ICANON)) {

            for (size_t i = 0; i < n; ++i) {
                tty->buffer[tty->loff] = seq[i];
                tty->loff = (tty->loff+1) % TTY_BUFF_LEN;
            }

            if (tty->termios.c_lflag & ECHO) {
                for (size_t i = 0; i < n; ++i) {
                    const char *esc = echo_map[(uint8_t)seq[i]];
                    petix_tty_write(tty, esc, strlen(esc));
                }
            }
        }


        if (seq[0] == '\n' || seq[0] == 0x04
            || !(tty->termios.c_lflag & ICANON)) {

            tty->buffer[tty->loff] = SEND_CHAR;
            tty->loff = (tty->loff+1) % TTY_BUFF_LEN;

            tty->lbase = tty->loff;
            cond_wake(&tty->read_sem);
        }
    }
}

ssize_t petix_tty_ioctl(struct petix_tty *tty, size_t req, va_list ap) {
    if (req == TCGETS) {
        struct termios *termios_p = va_arg(ap, void *);
        *termios_p = tty->termios;
        return 0;
    } else if (req == TCSETS || req == TCSETSW) {
        struct termios *termios_p = va_arg(ap, void *);
        acquire_lock(&tty->read_lock);
        tty->termios = *termios_p;
        release_lock(&tty->read_lock);
        return 0;
    } else if (req == TCSETSF) {
        acquire_lock(&tty->read_lock);
        struct termios *termios_p = va_arg(ap, void *);
        tty->fbase = tty->lbase = tty->loff;
        tty->termios = *termios_p;
        release_lock(&tty->read_lock);
        return 0;
    } else {
        return -ENOTTY;
    }
}
