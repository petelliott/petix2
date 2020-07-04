#ifndef SYS_IOCTL_H
#define SYS_IOCTL_H

enum ioctl_values {
    TCGETS,
    TCSETS,
    TCSETSW,
    TCSETSF,

    TIOCGWINSZ,
    TIOCSWINSZ,

    FB_IOCTL_SIZE,
    FB_IOCTL_GET_RESOLUTION,
};

struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;   /* unused */
    unsigned short ws_ypixel;   /* unused */
};


int ioctl(int fd, unsigned long request, ...);

#endif
