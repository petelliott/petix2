#include <unistd.h>
#include <sys/syscall.h>

int pipe(int filedes[2]) {
    return pipe2(filedes, 0);
}

int pipe2(int filedes[2], int flags) {
    return raw_syscall_errno(SYS_NR_PIPE, filedes, flags);
}
