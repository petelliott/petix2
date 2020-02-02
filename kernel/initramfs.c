#include "initramfs.h"
#include "tar.h"
#include <string.h>

#include "kdebug.h"


#define MAXFDS 100

static struct {
    const char *data;
    size_t len;
    size_t off;
} fdtable[MAXFDS];

static struct tar *fs;

void initramfs_init(struct tar *tar) {
    memset(fdtable, 0, sizeof(fdtable));
    fs = tar;
}

int initramfs_open(const char *pathname) {
    struct tar *tar;
    for (tar = (void *) fs; tar != NULL; tar = tar_next(tar)) {
        if (strcmp(tar->name, pathname) == 0) {
            break;
        }
    }
    if (tar == NULL) {
        return -1;
    }

    for (size_t i = 0; i < MAXFDS; ++i) {
        if (fdtable[i].data == NULL) {
            fdtable[i].data = tar_contents(tar);
            fdtable[i].len = tar_field(tar->size);
            fdtable[i].off = 0;
            return i;
        }
    }
    return -1;
}

int initramfs_close(int fd) {
    fdtable[fd].data = NULL;
    return 0;
}

ssize_t initramfs_read(int fd, void *buf, size_t count) {
    if (fd == -1 || fdtable[fd].data == NULL) {
        return -1;
    }
    if (fdtable[fd].off == fdtable[fd].len) {
        return 0;
    } else if (fdtable[fd].off + count >= fdtable[fd].len) {
        size_t nread = fdtable[fd].len - fdtable[fd].off;
        memcpy(buf, fdtable[fd].data + fdtable[fd].off, nread);
        fdtable[fd].off = fdtable[fd].len;
        return nread;
    } else {
        memcpy(buf, fdtable[fd].data + fdtable[fd].off, count);
        fdtable[fd].off += count;
        return count;
    }
}
