#ifndef ERRNO_H
#define ERRNO_H

extern int errno;

enum errno_ns {
    EPERM = 1,
    ENOENT = 2,
    ESRCH = 3,
    EINTR = 4,
    EIO   = 5,

    EISDIR = 21,
    EINVAL = 22,
};

#endif
