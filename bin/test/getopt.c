#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "abc:d:")) != -1) {
        if (optarg != NULL) {
            printf("-%c: \"%s\"\n", opt, optarg);
        } else {
            printf("-%c\n", opt);
        }
    }

    printf("args: ");
    for (size_t i = optind; i < argc; ++i) {
        printf("\"%s\" ", argv[i]);
    }
    printf("\n");
    return 0;
};
