#include "tar.h"
#include <stddef.h>

/* octal atoi for tar fields*/
size_t tar_field(const char *f) {
    size_t n = 0;
    for (size_t i = 0; f[i]; ++i) {
        n *= 8;
        n += f[i] - '0';
    }
    return n;
}


struct tar *tar_next(struct tar *header) {
    size_t size = tar_field(header->size);
    size_t fblocks = (size + TAR_BLOCKSIZE - 1) / TAR_BLOCKSIZE;
    struct tar *next = ((void *) header) + (fblocks + 1) * TAR_BLOCKSIZE;
    if (next->name[0] == 0) {
        return NULL;
    } else {
        return next;
    }
}

const char *empty = "";

const char *tar_contents(struct tar *header) {
    if (tar_field(header->size) == 0) {
        return empty;
    } else {
        return ((const char *) header) + TAR_BLOCKSIZE;
    }
}

size_t tar_next_blk(struct tar * header, size_t fblock) {
    size_t size = tar_field(header->size);
    size_t nblocks = (size + TAR_BLOCKSIZE - 1) / TAR_BLOCKSIZE;

    return fblock + nblocks +1;
}
