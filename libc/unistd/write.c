#include <unistd.h>
#include <sys/syscall.h>

ssize_t write(int fd, const void *buf, size_t count) {
    return raw_syscall_errno(SYS_NR_WRITE, fd, buf, count);
}
