#include "initrd.h"
#include "../device.h"
#include <string.h>

static void *start, *end;

static int open(struct inode *in, struct file *file, int flags) {
    file->size = end - start;
    return 0;
}

#define MIN(a, b) (((a)<(b))? (a):(b))

static ssize_t read(struct file *f, char *buf, size_t n) {
    size_t len = MIN(f->size - f->offset, n);
    memcpy(buf, start+f->offset, len);
    f->offset += len;
    return len;
}

static struct file_ops fops;

void initrd_init(void *s, void *e) {
    start = s;
    end   = e;

    memset(&fops, 0, sizeof(fops));
    fops = (struct file_ops) {
        .open = open,
        .lseek = fs_default_lseek,
        .read = read,
    };

    register_device(DEV_INITRD, &fops);
}
