#ifndef FS_H
#define FS_H

#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>

#define FILE_NAME_LEN 256
#define PATH_MAX 4096

struct inode;
struct file;
struct fs_inst;

struct dirent {
    char name[FILE_NAME_LEN];
    //TODO: other stuff?
};

struct file_ops {
    int (*open)(struct inode *, struct file *);

    off_t (*lseek)(struct file *, off_t, int);
    ssize_t (*read)(struct file *, char *, size_t);
    ssize_t (*write)(struct file *, const char *, size_t);

    int (*close)(struct file *);
};

struct inode_ops {
    struct file_ops reg_ops;

    int (*getroot)(const struct fs_inst *, struct inode *);

    int (*lookup)(struct inode *, const char *fname, struct inode *);

};

struct inode {
    bool exec : 1;
    enum {
        FT_DIR,
        FT_LINK,
        FT_SPECIAL,
        FT_REGULAR,
    } ftype;
    size_t inode_id;

    dev_t dev;
    const struct fs_inst *fs;
};


struct file {
    off_t offset;
    off_t size;
    struct inode inode;
    off_t private_data;
    const struct file_ops *fops;
};

struct fs_inst {
    struct file file;
    const struct inode_ops *iops;
};


int fs_mount(const char *targ, struct inode *src, const struct inode_ops *fs);
int fs_umount(const char *targ);

int fs_open(struct inode *in, struct file *f);

int fs_lookup(const char *path, struct inode *inode);

int register_device(int major, const struct file_ops *ops);

off_t fs_default_lseek(struct file *f, off_t off, int whence);

#endif
