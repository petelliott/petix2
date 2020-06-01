#include <fcntl.h>
#include <sys/syscall.h>


int open(const char *path, int flags, ...) {
    //TODO modes?
    return raw_syscall_errno(SYS_NR_OPEN, path, flags, 0);
}
