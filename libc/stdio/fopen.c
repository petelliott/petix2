#include "file.h"
#include <fcntl.h>
#include <string.h>

FILE *fopen(const char *path, const char *mode) {
    for (size_t i = 0; i < FOPEN_MAX; ++i) {
        if (!stdio_files[i].valid) {
            int oflags = 0;
            if (mode[strlen(mode) - 1] == '+') {
                oflags |= O_CREAT;
            }

            FILE *f = &(stdio_files[i]);
            f->fd = open(path, oflags, 0666);

            if (f->fd == -1) {
                return NULL;
            }

            f->err = 0;
            f->valid = true;
            f->eof = false;
            f->bf = _IOFBF;
            return f;
        }
    }
    errno = EAGAIN;
    return NULL;
}
