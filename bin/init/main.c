#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[]) {

    //TODO: have some kind of kernel tty
    int ttyfd = open("/dev/vgatty", 0, 0);
    dup2(ttyfd, STDIN_FILENO);
    dup2(ttyfd, STDOUT_FILENO);
    dup2(ttyfd, STDERR_FILENO);

    system("/etc/rc");

    FILE *ttys = fopen("/etc/ttys", "r");

    char buff[1024];
    while (!feof(ttys)) {
        fgets(buff, sizeof(buff), ttys);
        if (feof(ttys)) {
            break;
        }
        char *path = strtok(buff, " \n\t\r");

        pid_t getty_pid = fork();
        if (getty_pid == 0) {
            fclose(ttys);
            char *const gettyargv[] = {"/bin/getty", path, NULL};
            execve("/bin/getty", gettyargv, NULL);
        }
    }
    fclose(ttys);

    // wait for all children
    int wstatus;
    while (wait(&wstatus) != -1) {}

    // we probably wont return
    return 0;
}
