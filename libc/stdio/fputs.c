#include "file.h"
#include <string.h>

int fputs(const char *s, FILE *stream) {
    return fwrite(s, sizeof(char), strlen(s), stream);
}
