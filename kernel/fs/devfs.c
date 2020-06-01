#include "devfs.h"
#include <errno.h>
#include <string.h>
#include "../device.h"


static int dopen(struct inode *in, struct file *f) {
    return -EPERM;
}


static int lookup_all(struct fs_inst *fs, const char *path, struct inode *in) {
    in->exec = false;
    in->ftype = FT_SPECIAL;
    in->fs = fs;
    if (strcmp(path, "tty") == 0) {
        in->dev = MKDEV(DEV_TTY, 0);
    } else {
        return -ENOENT;
    }
    return 0;
}

const struct inode_ops *get_devfs(void) {
    static struct inode_ops iops;
    iops = (struct inode_ops) {
        .reg_ops = {
            .open = dopen,
        },
        .lookup_all = lookup_all,
    };
    return &iops;
}
