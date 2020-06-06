#include "file.h"
#include <stddef.h>

char *fgets(char *s, int size, FILE *stream) {
    for (size_t i = 0; i < size-1; ++i) {
        int ret = fgetc(stream);
        if (ret == EOF) {
            s[i] = 0;
            break;
        }

        s[i] = ret;

        if (ret == '\n') {
            s[i+1] = 0;
            break;
        }
    }
    return s;
}
