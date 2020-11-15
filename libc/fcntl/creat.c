#include <fcntl.h>
#include <sys/syscall.h>

int creat(const char *pathname, mode_t mode) {
    //TODO modes?
    return raw_syscall_errno(SYS_NR_CREAT, pathname);
}
