#include "file.h"

int fputc(int c, FILE *stream) {
    char ch = c;
    return fwrite(&ch, 1, sizeof(char), stream);
}
