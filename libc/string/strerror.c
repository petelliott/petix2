#include <string.h>
#include <errno.h>

static const char * const errtab[] = {
    [EPERM]  = "Operation not permitted",
    [ENOENT] = "No such file or directory",
    [ESRCH]  = "No such process",
    [EINTR]  = "Interrupted system call",
    [EIO]    = "Input/output error",
    [ENXIO]  = "No such device or address",
    [ENOEXEC] = "Exec format error",
    [EBADF]  = "Bad file descriptor",
    [ECHILD] = "No child processes",
    [EAGAIN] = "Resource temporarily unavailable",
    [EACCES] = "Permission denied",
    [EFAULT] = "Bad address",
    [EEXIST] = "File exists",
    [ENODEV] = "No such device",
    [ENOTDIR] = "Not a directory",
    [EISDIR] = "Is a directory",
    [EINVAL] = "Invalid argument",
    [EMFILE] = "Too many open files",
    [ENOTTY] = "Inappropriate ioctl for device",
    [ENOSYS] = "Function not Implemented",
    [ENOTSUP] = "Operation not supported",
};

const char *strerror(int errnum) {
    return errtab[errnum];
}
