#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // fake shell
    printf("$ ");

    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        write(STDOUT_FILENO, &c, 1);
        if (c == '\n') {
            printf("$ ");
        }
    }

    return 0;
}
