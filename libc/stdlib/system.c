#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int system(const char *command) {
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        // i'm pretty sure that command will not be modified
        char *const argv[] = {"/bin/sh", "-c", (char *)command, NULL};
        execve("/bin/sh", argv, NULL);
        perror("execve(2)");
        _exit(127);
    }

    int wstatus;
    waitpid(pid, &wstatus, 0);
    return wstatus;
}
