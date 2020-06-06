#include <stdio.h>
#include <string.h>
#include <errno.h>

void perror(const char *s) {
    if (s != NULL && *s != '\0') {
        fprintf(stderr, "%s: %s\n", s, strerror(errno));
    } else {
        fprintf(stderr, "%s\n", strerror(errno));
    }
}
