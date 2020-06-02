#include <sys/wait.h>
#include <sys/syscall.h>


pid_t wait(int *wstatus) {
    return waitpid(-1, wstatus, 0);
}

pid_t waitpid(pid_t pid, int *wstatus, int options) {
    return raw_syscall_errno(SYS_NR_WAITPID, pid, wstatus, options);
}
