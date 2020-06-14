#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "read.h"

int main(int argc, char *argv[]) {
    // fake shell
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


    char *args[32];
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

        char *prg = parse_line(buff, args);
        if (prg == NULL) {
            // ignore comments
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork(2)");
        } else if (pid == 0) {
            execvp(args[0], args);
            // if we are here, there must be an error
            perror("execvp(2)");

            return 1;
        } else {
            int wstatus;
            waitpid(pid, &wstatus, 0);
        }

    }
    fclose(f);
    if (prompt) {
        fputs("\ngoodbye.\n", stdout);
    }

    return 0;
}
