#include <unistd.h>
#include <sys/syscall.h>

int dup2(int fd, int fd2) {
    return raw_syscall(SYS_NR_DUP2, fd, fd2);
}
