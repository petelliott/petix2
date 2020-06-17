#ifndef SYS_IOCTL_H
#define SYS_IOCTL_H

enum ioctl_values {
    TCGETS,
    TCSETS,
};

int ioctl(int fd, unsigned long request, ...);

#endif
