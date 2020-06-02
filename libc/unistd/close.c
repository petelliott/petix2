#include <unistd.h>
#include <sys/syscall.h>

int close(int fd) {
    return raw_syscall_errno(SYS_NR_CLOSE, fd);
}
