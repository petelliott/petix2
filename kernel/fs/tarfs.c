#include "tarfs.h"
#include "../tar.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../kdebug.h"
#include "../sync.h"


static int topen(struct inode *in, struct file *f) {
    f->size = in->size;
    f->private_data = (in->inode_id + 1)*TAR_BLOCKSIZE;
    return 0;
}

#define MIN(a, b) (((a)<(b))? (a):(b))

static ssize_t tread(struct file *f, char *buf, size_t n) {
    size_t len = MIN(f->size - f->offset, n);

    off_t off = f->private_data + f->offset;

    acquire_lock(&(f->inode.fs->lock));
    f->inode.fs->file.fops->lseek(&(f->inode.fs->file), off, SEEK_SET);
    int ret = f->inode.fs->file.fops->read(&(f->inode.fs->file), buf, len);
    release_lock(&(f->inode.fs->lock));

    return ret;
}

#define ID_ROOT 0

static int getroot(struct fs_inst *fs, struct inode *in) {
   in->fs = fs;
   in->exec = false;
   in->ftype = FT_DIR;
   in->inode_id = ID_ROOT;

   return 0;
}

static int lookup_all(struct fs_inst *fs, const char *path, struct inode *in) {
    char buf[TAR_BLOCKSIZE];

    acquire_lock(&(fs->lock));

    fs->file.fops->lseek(&(fs->file), 0, SEEK_SET);

    struct tar *const tar = (void *) buf;

    for (size_t blk = 0;; blk = tar_next_blk(tar, blk)) {
        fs->file.fops->lseek(&(fs->file), blk*TAR_BLOCKSIZE, SEEK_SET);
        fs->file.fops->read(&(fs->file), buf, TAR_BLOCKSIZE);

        if (tar->name[0] == 0) {
            release_lock(&(fs->lock));
            return -ENOENT;
        }

        if (strncmp(tar->name, path, 100) == 0) {
            in->exec = 0; //TODO
            if (tar->typeflag == REGTYPE) {
                in->ftype = FT_REGULAR;
            } else if (tar->typeflag == DIRTYPE) {
                in->ftype = FT_DIR;
            } else if (tar->typeflag == CHRTYPE || tar->typeflag == BLKTYPE) {
                in->ftype = FT_SPECIAL;
                in->dev = MKDEV(tar_field(tar->devmajor),
                                tar_field(tar->devminor));
            } else {
                panic("unsupported tar file type");
            }
            in->size = tar_field(tar->size);
            in->inode_id = blk;
            in->fs = fs;

            release_lock(&(fs->lock));
            return 0;
        }
    }
    panic("unreachable code reached");
    return 0;
}

const struct inode_ops *get_tarfs(void) {
    static struct inode_ops iops;
    iops = (struct inode_ops) {
        .reg_ops = {
            .open = topen,
            .lseek = fs_default_lseek,
            .read = tread,
        },
        .getroot = getroot,
        .lookup_all = lookup_all
    };
    return &iops;
}
