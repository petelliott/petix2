#include <unistd.h>
#include <sys/syscall.h>


int execve(const char *path, char *const argv[], char *const envp[]) {
    return raw_syscall_errno(SYS_NR_EXEC, path, argv, envp);
}
