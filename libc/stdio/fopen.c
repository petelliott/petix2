#include "file.h"
#include <fcntl.h>

FILE *fopen(const char *path, const char *mode) {
    for (size_t i = 0; i < MAX_FILES; ++i) {
        if (!stdio_files[i].valid) {
            FILE *f = &(stdio_files[i]);
            f->fd = open(path, 0);

            if (f->fd == -1) {
                return NULL;
            }

            f->err = 0;
            f->valid = true;
            f->eof = false;
            return f;
        }
    }
    errno = EAGAIN;
    return NULL;
}
