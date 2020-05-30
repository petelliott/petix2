#include <string.h>
#include <errno.h>

static const char * const errtab[] = {
    [EPERM]  = "Operation not permitted",
    [ENOENT] = "No such file or directory",
    [ESRCH]  = "No such process",
    [EINTR]  = "Interrupted system call",
    [EIO]    = "Input/output error",
    [EISDIR] = "Is a directory",
    [EINVAL] = "Invalid argument",
};

const char *strerror(int errnum) {
    return errtab[errnum];
}
