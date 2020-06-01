#include <unistd.h>
#include <sys/syscall.h>


pid_t fork(void) {
    return raw_syscall_errno(SYS_NR_FORK);
}
