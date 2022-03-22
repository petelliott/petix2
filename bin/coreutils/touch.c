#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "touch: missing file argument");
    }

    for (int i = 1; i < argc; ++i) {
        int fd = open(argv[i], O_CREAT, 0666); // we don't actually support modes lol
        if (fd == -1) {
            perror("open(2)");
            return 1;
        }
        close(fd);
    }
    return 0;
}
