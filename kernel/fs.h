#ifndef FS_H
#define FS_H

#include <sys/types.h>

#define FILE_NAME_LEN 256

struct inode;
struct file;

struct file_ops {
    int (*open)(struct inode *, struct file *);

    off_t (*lseek)(struct file *, off_t, int);
    ssize_t (*read)(struct file *, char *, size_t);
    ssize_t (*write)(struct file *, const char *, size_t);

    int (*close)(struct file *);
};

struct file {
    off_t offset;
    off_t size;
    struct inode inode;
    off_t private_data;
    const struct file_ops *fops;
};

struct dirent {
    char name[FILE_NAME_LEN];
    //TODO: other stuff?
};

struct fs_inst {
    struct file;
    const struct inode_ops *iops;
};

struct inode_ops {
    struct file_ops reg_ops;

    int (*lookup) (const char *fname);
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

    // numbers for device files
    int major;
    int minor;

    const struct fs_inst *fs;
};


int fs_mount(const char *targ, struct inode *src, const struct inode_ops *fs);
int fs_umount(const char *targ);

int fs_open(struct inode *in);

int register_device(int major, const struct inode_ops *ops);



#endif
