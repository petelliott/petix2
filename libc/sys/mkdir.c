#include <sys/stat.h>
#include <sys/syscall.h>

int mkdir(const char *pathname, mode_t mode) {
    //TODO modes?
    return raw_syscall_errno(SYS_NR_MKDIR, pathname);
}
