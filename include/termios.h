#ifndef TERMIOS_H
#define TERMIOS_H

#include <stdint.h>

#define NCCS 1

#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

typedef uint32_t tcflag_t;
typedef char     cc_t;

struct termios {
    tcflag_t c_iflag;      /* input modes */
    tcflag_t c_oflag;      /* output modes */
    tcflag_t c_cflag;      /* control modes */
    tcflag_t c_lflag;      /* local modes */
    cc_t     c_cc[NCCS];   /* special characters */
};

enum c_iflag_flags {
    TODO_i,
};

enum c_oflag_flags {
    TODO_o,
};

enum c_cflag_flags {
    ICANON,
    ECHO,
    ISIG,
};

enum c_cc_chars {
    TODO_cc,
};



#endif
