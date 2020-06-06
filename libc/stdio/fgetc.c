#include "file.h"

int fgetc(FILE *stream) {
    char ch;
    if (fread(&ch, sizeof(ch), 1, stream) <= 0) {
        return EOF;
    }

    return ch;
}
