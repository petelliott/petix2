#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

static bool noflags = true;
static bool lp = false;
static bool wp = false;
static bool cp = false;

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
    if (lp) printf("%i ", l);
    if (wp) printf("%i ", w);
    if (cp) printf("%i ", c);
    printf("%s\n", name);
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
    int opt;
    while ((opt = getopt(argc, argv, "lwc")) != -1) {
        noflags = false;
        if (opt == 'l') {
            lp = true;
        } else if (opt == 'w') {
            wp = true;
        } else if (opt == 'c') {
            cp = true;
        }
    }

    if (noflags) {
        lp = wp = cp = true;
    }

    if (argc-optind == 0) {
        wc_f(stdin, "");
    } else {
        for (size_t i = optind; i < argc; ++i) {
            wc(argv[i]);
        }
    }

    return 0;
}
