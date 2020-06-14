#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    int filedes[2];
    if (pipe(filedes) == -1) {
        perror("pipe(2)");
        return 1;
    }


    pid_t pid = fork();
    if (pid == 0) {
        // child
        char buff[1024];
        int ret;
        while ((ret = read(filedes[0], buff, 4)) > 0) {
            buff[ret] = 0;
            printf("%i, '%s'\n", ret, buff);
        }

        if (ret == -1) {
            perror("read(2)");
        }

        close(filedes[0]);
    } else if (pid > 0) {
        // parent
        for (size_t i = 0; i < 12; ++i) {
            write(filedes[1], "abc", 3);
        }
        close(filedes[1]);

        int wstatus;
        waitpid(pid, &wstatus, 0);
    } else {
        perror("fork(2)");
        return 1;
    }

    return 0;
}
