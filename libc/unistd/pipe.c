#include <unistd.h>
#include <sys/syscall.h>

int pipe(int filedes[2]) {
    return raw_syscall_errno(SYS_NR_PIPE, filedes);
}
