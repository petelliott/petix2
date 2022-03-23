#ifndef FS_H
#define FS_H

#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include "sync.h"

#define PATH_MAX 4096

struct inode;
struct file;
struct fs_inst;

struct file_ops {
    int (*open)(struct inode *, struct file *, int);

    off_t (*lseek)(struct file *, off_t, int);
    ssize_t (*read)(struct file *, char *, size_t);
    ssize_t (*write)(struct file *, const char *, size_t);
    int (*getdent)(struct file *, struct petix_dirent *);
    int (*ioctl)(struct file *, unsigned long, va_list);
    void *(*mmap)(struct file *, void *, size_t, int, int, off_t, int *);

    int (*close)(struct file *);
};

struct inode_ops {
    struct file_ops reg_ops;

    int (*getroot)(struct fs_inst *, struct inode *);

    int (*lookup)(struct inode *, const char *fname, struct inode *);
    int (*lookup_all)(struct fs_inst *, const char *path, struct inode *);

    int (*creat)(struct inode *, const char *name);
    int (*mkdir)(struct inode *, const char *name);
};

struct inode {
    bool exec : 1;
    enum {
        FT_DIR,
        FT_LINK,
        FT_SPECIAL,
        FT_REGULAR,
    } ftype;
    size_t size; // optional. for use by file systems
    size_t inode_id;

    dev_t dev;
    struct fs_inst *fs;
};

struct file {
    off_t offset;
    off_t size;
    struct inode inode;
    off_t private_data;
    const struct file_ops *fops;
    long refcnt; // used only for file descriptor files
};

struct fs_inst {
    struct file file;
    petix_lock_t lock; //for use by the fs
    const struct inode_ops *iops;
    void *private_data;
};


int fs_mount(const char *targ, struct inode *src, const struct inode_ops *fs);
int fs_umount(const char *targ);

int fs_open(struct inode *in, struct file *f, int flags);

int fs_lookup(const char *path, struct inode *inode);

int register_device(int major, const struct file_ops *ops);

off_t fs_default_lseek(struct file *f, off_t off, int whence);

#endif
