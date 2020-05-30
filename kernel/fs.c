#include "fs.h"
#include "kdebug.h"
#include <errno.h>


static struct file_ops const * devices[256];

int fs_mount(const char *targ, struct inode *src, const struct inode_ops *fs);
int fs_umount(const char *targ);

int fs_open(struct inode *in, struct file *f) {
    if (in->ftype == FT_SPECIAL) {
        const struct file_ops *fops = devices[MAJOR(in->dev)];
        if (fops == NULL) {
            return -ENXIO;
        }

        f->inode = *in;
        f->offset = 0;
        f->fops = fops;
        return fops->open(in, f);
    } else if (in->ftype == FT_REGULAR) {
        f->inode = *in;
        f->offset = 0;
        f->fops = &(in->fs->iops->reg_ops);
        return f->fops->open(in, f);
    } else if (in->ftype == FT_DIR) {
        return -EISDIR;
    } else if (in->ftype == FT_LINK) {
        panic("symlinks are not supported yet");
    }
    return -EINVAL;
}

int register_device(int major, const struct file_ops *ops) {
    kassert(major < 256 && major >= 0);
    devices[major] = ops;
    return 0;
}
