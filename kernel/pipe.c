#include "pipe.h"
#include "kmalloc.h"
#include "string.h"
#include <errno.h>
#include "kdebug.h"

#define READ_END 0x100000000ll
#define WRITE_END 0x200000000ll

#define MIN(a, b) ((a < b)? a:b)

static ssize_t pread(struct file *f, char *buf, size_t count) {
    if (f->private_data == 0) {
        return 0;
    }

    if (!(f->private_data & READ_END)) {
        return -EPERM;
    }

    struct pipe *pipe = (void *)(uintptr_t) (f->private_data & 0xffffffff);

    ssize_t c = count;
    while (count > 0) {
        if (!pipe->owrite && pipe->hi == pipe->lo) {
            return c-count;
        }

        acquire_lock(&(pipe->lock));
        while (pipe->hi != pipe->lo && count > 0) {
            *buf = pipe->buffer[pipe->lo];
            ++buf;
            --count;
            pipe->lo = (pipe->lo + 1) % PIPE_SIZE;
        }

        if ((pipe->hi+1)%PIPE_SIZE != pipe->lo) {
            cond_wake(&(pipe->wcond));
        }

        release_lock(&(pipe->lock));

        if (count > 0) {
            cond_wait(&(pipe->rcond));
        }
    }

    return c;
}

static ssize_t pwrite(struct file *f, const char *buf, size_t count) {
    if (f->private_data == 0) {
        return 0;
    }

    if (!(f->private_data & WRITE_END)) {
        return -EPERM;
    }

    struct pipe *pipe = (void *)(uintptr_t) (f->private_data & 0xffffffff);

    ssize_t c = count;
    while (count > 0) {
        if (!pipe->oread) {
            return c-count;
        }

        acquire_lock(&(pipe->lock));
        while ((pipe->hi+1)%PIPE_SIZE != pipe->lo && count > 0) {
            pipe->buffer[pipe->hi] = *buf;
            ++buf;
            --count;
            pipe->hi = (pipe->hi + 1) % PIPE_SIZE;
        }

        if (pipe->hi != pipe->lo) {
            cond_wake(&(pipe->rcond));
        }

        release_lock(&(pipe->lock));

        if (count > 0) {
            cond_wait(&(pipe->wcond));
        }
    }

    return c;
}

static int pclose(struct file *f) {
    if (f->private_data == 0) {
        // pipe is already closed and freed
        return 0;
    }

    struct pipe *pipe = (void *)(uintptr_t) (f->private_data & 0xffffffff);
    acquire_lock(&(pipe->lock));

    if (f->private_data & READ_END) {
        pipe->oread = false;
    } else if (f->private_data & WRITE_END) {
        pipe->owrite = false;
    }

    cond_wake(&(pipe->wcond));
    cond_wake(&(pipe->rcond));

    release_lock(&(pipe->lock));
    if (!pipe->oread && !pipe->owrite) {
        kfree_sync(pipe);
        f->private_data = 0;
    }

    return 0;
}


void open_pipe(struct file *rfile, struct file *wfile) {
    static struct file_ops fops;

    struct pipe *pipe = kmalloc_sync(sizeof(struct pipe));
    memset(pipe, 0, sizeof(struct pipe));

    pipe->oread = true;
    pipe->owrite = true;

    fops = (struct file_ops) {
        .open = NULL,
        .lseek = NULL,
        .read = pread,
        .write = pwrite,
        .getdent = NULL,
        .close = pclose,
    };

    rfile->fops = &fops;
    rfile->private_data = READ_END | (off_t) (uintptr_t) pipe;

    wfile->fops = &fops;
    wfile->private_data = WRITE_END | (off_t) (uintptr_t) pipe;

}
