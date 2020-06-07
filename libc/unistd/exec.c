#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>


int execve(const char *path, char *const argv[], char *const envp[]) {
    return raw_syscall_errno(SYS_NR_EXEC, path, argv, envp);
}

int execvp(const char *path, char *const argv[]) {
    //TODO make this for real
    if (strchr(path, '/')) {
        return execve(path, argv, NULL);
    } else {
        char buff[1024] = {'/', 'b', 'i', 'n', '/', '\0'};
        strncat(buff, path, sizeof(buff));
        return execve(buff, argv, NULL);
    }
}
