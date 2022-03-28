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
    d->type = DT_SPECIAL;
    if (f->offset == 0) {
        d->inode_id = 0;
        d->present = true;
        strncpy(d->name, "vgatty", sizeof(d->name));
    } else if (f->offset == 1) {
        d->inode_id = 1;
        d->present = true;
        strncpy(d->name, "comtty", sizeof(d->name));
    } else if (f->offset == 2) {
        d->inode_id = 2;
        d->present = true;
        strncpy(d->name, "fb", sizeof(d->name));
    } else if (f->offset == 3) {
        d->inode_id = 3;
        d->present = true;
        strncpy(d->name, "fbtty", sizeof(d->name));
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

    if (strcmp(path, "vgatty") == 0) {
        in->dev = MKDEV(DEV_VGATTY, 0);
    } else if (strcmp(path, "comtty") == 0) {
        in->dev = MKDEV(DEV_COMTTY, 0);
    } else if (strcmp(path, "fb") == 0) {
        in->dev = MKDEV(DEV_FB, 0);
    } else if (strcmp(path, "fbtty") == 0) {
        in->dev = MKDEV(DEV_FBTTY, 0);
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
