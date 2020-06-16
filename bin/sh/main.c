#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

pid_t exec_prog(char *argv[], int in, int out) {
    pid_t pid = fork();
    if (pid == 0) {
        dup2(in, STDIN_FILENO);
        dup2(out, STDOUT_FILENO);

        execvp(argv[0], argv);
        // if we are here, there must be an error
        perror("execvp(2)");

        //TODO: write exit
        _exit(1);
    }
    return pid;
}

pid_t do_prog(char *prog, int in, int out) {

    char *args[32];

    char *save;

    size_t i = 0;
    for (char *c = strtok_r(prog, " \t\n", &save); c != NULL;
         c = strtok_r(NULL, " \t\n", &save)) {

        args[i++] = c;
    }
    args[i] = NULL;
    return exec_prog(args, in, out);
}

void do_line(char *line) {
    for (; *line == ' ' || *line == '\t' || *line == '\n'; ++line) {}
    if (*line == '#' || *line == '\0') {
        return;
    }

    char *save;

    char *prev = strtok_r(line, "|", &save);
    int infd = STDIN_FILENO;

    for (char *c = strtok_r(NULL, "|", &save); c != NULL;
         c = strtok_r(NULL, "|", &save)) {

        int filedes[2];
        pipe2(filedes, O_CLOEXEC);
        pid_t pid = do_prog(prev, infd, filedes[1]);
        if (pid == -1) {
            perror("fork(2)");
            return;
        }

        if (infd != STDIN_FILENO) {
            close(infd);
        }
        close(filedes[1]);

        infd = filedes[0];

        prev = c;
    }

    pid_t pid = do_prog(prev, infd, STDOUT_FILENO);
    if (pid == -1) {
        perror("fork(2)");
        return;
    }

    int wstatus;
    waitpid(pid, &wstatus, 0);
}

int main(int argc, char *argv[]) {
    bool prompt;
    FILE *f;

    if (argc == 1) {
        prompt = true;
        f = stdin;
    } else if (argc == 2) {
        prompt = false;
        f = fopen(argv[1], "r");
        if (f == NULL) {
            perror("fopen(3)");
            return 1;
        }
    } else {
        fprintf(stderr, "usage: %s [script]\n", argv[0]);
        return 1;
    }


    char buff[1024];
    while (!feof(f)) {
        if (prompt) {
            printf("$ ");
            fflush(stdout);
        }
        fgets(buff, sizeof(buff), f);

        if (feof(stdin)) {
            break;
        }

        //fprintf(stderr, "buff=\"%s\"\n", buff);

        do_line(buff);

    }
    fclose(f);
    if (prompt) {
        fputs("\ngoodbye.\n", stdout);
    }

    return 0;
}
