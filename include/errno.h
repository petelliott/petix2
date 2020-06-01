#ifndef ERRNO_H
#define ERRNO_H

extern int errno;

enum errno_ns {
    EPERM  = 1,
    ENOENT = 2,
    ESRCH  = 3,
    EINTR  = 4,
    EIO    = 5,
    ENXIO  = 6,

    EBADF  = 9,

    EAGAIN = 11,

    ENOTDIR = 20,
    EISDIR  = 21,
    EINVAL  = 22,

    EMFILE  = 24,

    ENOSYS  = 38,
};

#endif
