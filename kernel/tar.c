#include "tar.h"
#include <stddef.h>

/* octal atoi for tar fields*/
static size_t ftonum(const char *f) {
    size_t n = 0;
    for (size_t i = 0; f[i]; ++i) {
        n *= 8;
        n += f[i] - '0';
    }
    return n;
}


struct tar *tar_next(struct tar *header) {
    size_t size = ftonum(header->size);
    size_t fblocks = (size + BLOCKSIZE - 1) / BLOCKSIZE;
    struct tar *next = ((void *) header) + (fblocks + 1) * BLOCKSIZE;
    if (next->name[0] == 0) {
        return NULL;
    } else {
        return next;
    }
}

const char *empty = "";

const char *tar_contents(struct tar *header) {
    if (ftonum(header->size) == 0) {
        return empty;
    } else {
        return ((const char *) header) + BLOCKSIZE;
    }
}
