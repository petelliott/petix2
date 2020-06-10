#include "devfs.h"
#include <errno.h>
#include <string.h>
#include "../device.h"

enum inode_ids {
    ID_ROOT,
    ID_OTHER,
};

static int dopen(struct inode *in, struct file *f, int flags) {
    if (in->inode_id == ID_ROOT) {
        return 0;
    }

    return -EPERM;
}

static int getroot(struct fs_inst *fs, struct inode *in) {
    in->fs = fs;
    in->exec = false;
    in->ftype = FT_DIR;
    in->inode_id = ID_ROOT;
    return 0;
}

static int getdent(struct file *f, struct petix_dirent *d) {
    if (f->offset == 0) {
        d->inode_id = 0;
        d->present = true;
        strncpy(d->name, "tty", sizeof(d->name));
    } else {
        d->present = false;
    }
    f->offset++;
    return 0;
}

static int lookup_all(struct fs_inst *fs, const char *path, struct inode *in) {
    in->exec = false;
    in->ftype = FT_SPECIAL;
    in->fs = fs;
    in->inode_id = ID_OTHER;

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
            .getdent = getdent,
        },
        .getroot = getroot,
        .lookup_all = lookup_all,
    };
    return &iops;
}
