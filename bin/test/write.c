#include <stdio.h>

void w_f(FILE *f) {
    char buff[1024];
    while (!feof(stdin)) {
        size_t n = fread(buff, sizeof(char), sizeof(buff), stdin);
        fwrite(buff, sizeof(char), n, f);
    }
}

void w(const char *file) {
    if (file[0] == '-' && file[1] == '\0') {
        w_f(stdout);
    } else {
        FILE *f = fopen(file, "w+");
        if (f == NULL) {
            perror("fopen(3)");
            return;
        }
        w_f(f);
        fclose(f);
    }

}

int main (int argc, char *argv[]) {

    if (argc == 1) {
        w_f(stdout);
    } else {
        w(argv[1]);
    }

    return 0;
}
