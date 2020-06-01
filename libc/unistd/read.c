#include <unistd.h>
#include <sys/syscall.h>

ssize_t read(int fd, void *buf, size_t count) {
    return raw_syscall_errno(SYS_NR_READ, fd, buf, count);
}
