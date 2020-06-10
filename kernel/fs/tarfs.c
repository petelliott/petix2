#include "tarfs.h"
#include "../tar.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../kdebug.h"
#include "../sync.h"

#define ID_ROOT 0xffffffff

static int topen(struct inode *in, struct file *f, int flags) {
    f->size = in->size;
    if (in->inode_id == ID_ROOT) {
        f->private_data = 0;
    } else {
        f->private_data = (in->inode_id + 1)*TAR_BLOCKSIZE;
    }
    return 0;
}

#define MIN(a, b) (((a)<(b))? (a):(b))

static ssize_t tread(struct file *f, char *buf, size_t n) {
    size_t len = MIN(f->size - f->offset, n);

    off_t off = f->private_data + f->offset;

    acquire_lock(&(f->inode.fs->lock));
    f->inode.fs->file.fops->lseek(&(f->inode.fs->file), off, SEEK_SET);
    int ret = f->inode.fs->file.fops->read(&(f->inode.fs->file), buf, len);
    if (ret > 0) {
        f->offset += ret;
    }
    release_lock(&(f->inode.fs->lock));

    return ret;
}

int getdent(struct file *f, struct petix_dirent *d) {
    if (f->inode.ftype != FT_DIR) {
        return -ENOTDIR;
    }

    acquire_lock(&(f->inode.fs->lock));

    char name[100];
    if (f->inode.inode_id == ID_ROOT) {
        name[0] = 0;
    } else {
        off_t off = f->private_data - TAR_BLOCKSIZE;

        f->inode.fs->file.fops->lseek(&(f->inode.fs->file), off, SEEK_SET);
        f->inode.fs->file.fops->read(&(f->inode.fs->file), name, sizeof(name));
    }

    char buf[TAR_BLOCKSIZE];

    struct tar *const tar = (void *) buf;

    size_t initblk = (f->private_data + f->offset)/TAR_BLOCKSIZE;
    for (size_t blk = initblk;; blk = tar_next_blk(tar, blk)) {
        f->inode.fs->file.fops->lseek(&(f->inode.fs->file),
                                      blk*TAR_BLOCKSIZE, SEEK_SET);
        f->inode.fs->file.fops->read(&(f->inode.fs->file), buf, TAR_BLOCKSIZE);

        f->offset = blk*TAR_BLOCKSIZE;

        size_t prefix_len = strnlen(name, sizeof(name));

        if (tar->name[0] == '\0' || strncmp(tar->name, name, prefix_len) != 0) {
            d->present = false;
            release_lock(&(f->inode.fs->lock));
            return 0;
        }

        char * s = strchr(tar->name + prefix_len, '/');
        if (s == NULL || (s[0] == '/' && s[1] == '\0')) {
            d->inode_id = blk;
            d->present = true;
            strncpy(d->name, tar->name + prefix_len, sizeof(name));

            blk = tar_next_blk(tar, blk);
            f->offset = blk*TAR_BLOCKSIZE - f->private_data;

            release_lock(&(f->inode.fs->lock));
            return 0;
        }
    }
    panic("unreachable code");
}


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

    struct tar *const tar = (void *) buf;

    for (size_t blk = 0;; blk = tar_next_blk(tar, blk)) {
        fs->file.fops->lseek(&(fs->file), blk*TAR_BLOCKSIZE, SEEK_SET);
        fs->file.fops->read(&(fs->file), buf, TAR_BLOCKSIZE);

        if (tar->name[0] == 0) {
            release_lock(&(fs->lock));
            return -ENOENT;
        }

        size_t pathlen = strnlen(path, 100);
        if (strncmp(tar->name, path, 100) == 0 ||
            (strncmp(tar->name, path, pathlen) == 0
             && tar->name[pathlen] == '/'
             && tar->name[pathlen+1] == '\0')) {

            in->exec = (tar_field(tar->mode) & 0111) != 0;
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
            .getdent = getdent
        },
        .getroot = getroot,
        .lookup_all = lookup_all
    };
    return &iops;
}
