#include <stdio.h>

int main(int argc, char *argv[]) {
    for (size_t i = 1; i < argc; ++i) {
        if (i > 1) {
            fputc(' ', stdout);
        }
        fputs(argv[i], stdout);
    }
    fputc('\n', stdout);

    return 0;
}
