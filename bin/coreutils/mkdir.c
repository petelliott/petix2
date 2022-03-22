#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "mkdir: missing directory argument\n");
    }

    for (int i = 1; i < argc; ++i) {
        if (mkdir(argv[i], 0666) == -1) {
            perror("open(2)");
            return 1;
        }
    }
    return 0;
}
