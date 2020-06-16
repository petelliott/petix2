#include <stdio.h>
#include <stdbool.h>

void wc_f(FILE *f, const char *name) {
    int l=0,w=0,c=0;

    bool inw=false;

    int ch;
    while ((ch=fgetc(f)) != EOF) {
        c++;
        if (ch == '\n') {
            l++;
        }

        if (ch == ' ' || ch == '\t' || ch == '\n') {
            if (inw) {
                w++;
            }
            inw = false;
        } else {
            inw = true;
        }
    }
    if (inw) {
        w++;
    }

    printf("%i %i %i %s\n", l, w, c, name);
}

void wc(const char *file) {
    if (file[0] == '-' && file[1] == '\0') {
        wc_f(stdin, "-");
    } else {
        FILE *f = fopen(file, "r");
        if (f == NULL) {
            perror("fopen(3)");
            return;
        }
        wc_f(f, file);
        fclose(f);
    }
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        wc_f(stdin, "");
    } else {
        for (size_t i = 1; i < argc; ++i) {
            wc(argv[i]);
        }
    }

    return 0;
}
