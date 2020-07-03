#include <stdlib.h>

static int atou(const char *nptr) {
    int n = 0;
    for (int i = 0; nptr[i] != '\0'; ++i) {
        n *= 10;
        n += (nptr[i] - '0');
    }
    return n;
}

int atoi(const char *nptr) {
    if (nptr[0] == '-') {
        return -atou(nptr + 1);
    }

    return atou(nptr);
}
