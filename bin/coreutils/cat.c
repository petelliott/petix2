#include <stdio.h>

void cat_f(FILE *f) {
    char buff[1024];
    while (!feof(f)) {
        size_t n = fread(buff, sizeof(char), sizeof(buff), f);
        fwrite(buff, sizeof(char), n, stdout);
    }
}

void cat(const char *file) {
    if (file[0] == '-' && file[1] == '\0') {
        cat_f(stdin);
    } else {
        FILE *f = fopen(file, "r");
        if (f == NULL) {
            perror("fopen(3)");
            return;
        }
        cat_f(f);
        fclose(f);
    }

}

int main (int argc, char *argv[]) {

    if (argc == 1) {
        cat_f(stdin);
    } else {
        for (size_t i = 1; i < argc; ++i) {
            cat(argv[i]);
        }
    }


    return 0;
}
