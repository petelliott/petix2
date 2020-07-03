#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>


int main(int argc, char *argv[]) {

    if (argc != 2) {
        raw_syscall(SYS_NR_DB_PRINT, "wrong number of args to getty");
        return 1;
    }

    int ttyfd = open(argv[1], 0, 0);
    if (ttyfd == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
        return 1;
    }

    int motdfd = open("/etc/motd", 0, 0);

    char buff[64];
    ssize_t len;
    while ((len = read(motdfd, buff, sizeof(buff))) > 0) {
        write(ttyfd, buff, len);
    }

    close(motdfd);

    dup2(ttyfd, STDIN_FILENO);
    dup2(ttyfd, STDOUT_FILENO);
    dup2(ttyfd, STDERR_FILENO);
    close(ttyfd);

    char *const nargv[] = {"/bin/sh", NULL};
    execve("/bin/sh", nargv, NULL);

    return 0;
}
