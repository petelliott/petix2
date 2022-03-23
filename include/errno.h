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

    ENOEXEC = 8,
    EBADF  = 9,
    ECHILD = 10,

    EAGAIN = 11,

    EACCES = 13,
    EFAULT = 14,

    EEXIST = 17,

    ENODEV  = 19,
    ENOTDIR = 20,
    EISDIR  = 21,
    EINVAL  = 22,

    EMFILE  = 24,
    ENOTTY  = 25,

    ENOSYS  = 38,

    ENOTSUP = 95,
};

#endif
