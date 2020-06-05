#include <unistd.h>

int main(int argc, char *argv[]) {
    // fake shell
    write(STDOUT_FILENO, "$ ", 2);
    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        write(STDOUT_FILENO, &c, 1);
        if (c == '\n') {
            write(STDOUT_FILENO, "$ ", 2);
        }
    }

    return 0;
}
