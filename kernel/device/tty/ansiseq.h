#ifndef DEVICE_TTY_ANSISEQ_H
#define DEVICE_TTY_ANSISEQ_H

#include <stdbool.h>
#include <stdint.h>
#include "ttylib.h"

enum ansi_color {
    ANSI_BLACK   = 0,
    ANSI_RED     = 1,
    ANSI_GREEN   = 2,
    ANSI_YELLOW  = 3,
    ANSI_BLUE    = 4,
    ANSI_MAGENTA = 5,
    ANSI_CYAN    = 6,
    ANSI_WHITE   = 7,

    ANSI_DEFAULT = 9,
};

struct ansi_rendition {
    enum ansi_color fg, bg;
    union {
        struct {
            bool na_1      : 1;
            bool bold      : 1;
            bool faint     : 1;
            bool italics   : 1;
            bool underline : 1;
            bool blink     : 1;
            bool na_2      : 1;
            bool negative  : 1;
        } f;
        uint8_t bits;
    } flags;
};

struct ansi_backend {
    void (*putch)(const struct ansi_rendition *, char ch, int row, int col);
    void (*curto)(int row, int col);
    void (*scroll_up)(void);
    int row_n, col_n;
};

enum ansi_state {
    ANSI_S_NONE,
    ANSI_S_ESC,
    ANSI_S_ESC_LBRAK,
};

struct ansi_term {
    int row, col;

    //parsing stuff
    int numerics[16];
    int noff;
    bool nump;
    enum ansi_state state;

    struct ansi_rendition rendition;
    const struct ansi_backend *backend;
    struct petix_tty *tty;
};
void ansi_init(struct ansi_term *term);
void ansi_putch(struct ansi_term *term, char ch);


#endif
