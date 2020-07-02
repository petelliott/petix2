#include <stdio.h>
#include <unistd.h>

const char escapes[256] = {
    ['\\'] = '\\',
    ['a'] = '\a',
    ['b'] = '\b',
    ['e'] = '\e',
    ['f'] = '\f',
    ['n'] = '\n',
    ['r'] = '\r',
    ['t'] = '\t',
    ['v'] = '\v',
};

void fputs_escape(const char *s, FILE *f) {
    for (size_t i = 0; s[i] != '\0'; ++i) {
        if (s[i] == '\\') {
            ++i;
            if (s[i] == '\0' || s[i] == 'c'){
                break;
            } else if (escapes[(int)s[i]]) {
                fputc(escapes[(int)s[i]], stdout);
            } else {
                fputc(s[i], stdout);
            }
        } else {
            fputc(s[i], stdout);
        }
    }
}

int main(int argc, char *argv[]) {
    bool optn = false;
    bool opte = false;

    int opt;
    while ((opt = getopt(argc, argv, "en")) != -1) {
        if (opt == 'e') {
            opte = true;
        } else if (opt == 'n') {
            optn = true;
        }
    }

    for (size_t i = optind; i < argc; ++i) {
        if (i > optind) {
            fputc(' ', stdout);
        }
        if (opte) {
            fputs_escape(argv[i], stdout);
        } else {
            fputs(argv[i], stdout);
        }
    }

    if (!optn) {
        fputc('\n', stdout);
    }

    fflush(stdout);

    return 0;
}
