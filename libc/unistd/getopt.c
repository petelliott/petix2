#include <unistd.h>
#include <stdio.h>
#include <string.h>

static int nextchar = -1;

int getopt(int argc, char *argv[], const char *optstring) {
    // TODO optstring[0] = ":"
    if (optind >= argc) {
        return -1;
    }

    if (nextchar != -1 && argv[optind][nextchar] == '\0') {
        optind++;
        nextchar = -1;
    }

    size_t next = optind;
    if (nextchar == -1) {
        // permute next option into optind slot
        for (next = optind; next < argc && argv[next][0] != '-'; ++next) {}

        if (next >= argc || argv[next][1] == '-') {
            optind++;
            return -1;
        }

        for (size_t i = next; i > optind; --i) {
            char * const tmp = argv[i];
            argv[i] = argv[i-1];
            argv[i-1] = tmp;
        }

        nextchar = 1;
    }

    optarg = NULL;
    char *optch = argv[optind]+nextchar;
    char *arg = strchr(optstring, *optch);
    if (arg == NULL) {
        nextchar++;
        fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], *optch);
        return '?';
    } else if (arg[1] == ':') {
        if (optch[1] == '\0') {
            for (size_t i = next+1; i > optind+1; --i) {
                char * const tmp = argv[i];
                argv[i] = argv[i-1];
                argv[i-1] = tmp;
            }

            optarg = argv[optind+1];
            nextchar = -1;
            optind += 2;
            if (optarg == NULL) {
                fprintf(stderr, "%s: option requires an argument -- '%c'\n",
                        argv[0], *optch);
                return '?';
            }
            return *optch;
        } else {

            optarg = optch+1;
            nextchar = -1;
            optind += 1;
            return *optch;
        }
    } else {
        nextchar++;
        return *optch;
    }
}

char *optarg;
int optind = 1;
int opterr;
int optopt;
