#include "fs.h"
#include "kdebug.h"
#include <errno.h>
#include <unistd.h>
#include "sync.h"
#include "kmalloc.h"
#include <string.h>
#include <fcntl.h>


static struct file_ops const * devices[256];

int fs_open(struct inode *in, struct file *f, int flags) {
    if (in->ftype == FT_SPECIAL) {
        if (flags & O_DIRECTORY) {
            return -ENOTDIR;
        }
        const struct file_ops *fops = devices[MAJOR(in->dev)];
        if (fops == NULL) {
            return -ENXIO;
        }

        f->inode = *in;
        f->offset = 0;
        f->fops = fops;
        kassert(fops->open != NULL);

        return fops->open(in, f, flags);
    } else if (in->ftype == FT_REGULAR) {
        if (flags & O_DIRECTORY) {
            return -ENOTDIR;
        }
        f->inode = *in;
        f->offset = 0;
        f->fops = &(in->fs->iops->reg_ops);
        kassert(f->fops->open != NULL);

        return f->fops->open(in, f, flags);
    } else if (in->ftype == FT_DIR) {
        if (!(flags & O_DIRECTORY)) {
            return -EISDIR;
        }
        f->inode = *in;
        f->offset = 0;
        f->fops = &(in->fs->iops->reg_ops);
        kassert(f->fops->open != NULL);

        return f->fops->open(in, f, flags);
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

off_t fs_default_lseek(struct file *f, off_t off, int whence) {
    if (whence == SEEK_SET) {
        f->offset = off;
    } else if (whence == SEEK_CUR) {
        f->offset += off;
    } else if (whence == SEEK_END) {
        f->offset = f->size + off;
    } else {
        return -EINVAL;
    }

    return f->offset;
}


petix_lock_t mount_lock;

struct mount_tree {
    char name[FILE_NAME_LEN];
    bool mountpoint;
    struct fs_inst fs;
    struct mount_tree *child;
    struct mount_tree *sibling;
};

struct mount_tree mount_root;

static int fs_getfs(const char *p, struct fs_inst **fs, const char **relpath) {

    char path[PATH_MAX];
    strncpy(path, p, PATH_MAX);
    char *save;

    struct mount_tree *node = &mount_root;

    ssize_t reloff = -1;

    if (node->mountpoint) {
        *fs = &(node->fs);
        reloff = 0;
    }

    for (char *i = strtok_r(path, "/", &save); i != NULL;
         i = strtok_r(NULL, "/", &save)) {

        bool found = false;
        for (struct mount_tree *j = node->child; j != NULL; j = j->sibling) {
            if (strncmp(j->name, i, FILE_NAME_LEN) == 0) {
                found = true;
                node = j;
                break;
            }
        }

        if (!found) {
            break;
        }

        if (node->mountpoint) {
            *fs = &(node->fs);
            reloff = save - path;
        }
    }

    if (reloff == -1) {
        return -ENOENT;
    }
    *relpath = p + reloff;
    return 0;
}

int fs_lookup(const char *p, struct inode *inode) {
    acquire_lock(&mount_lock);

    const char *p2;
    struct fs_inst *fs;
    int ret = fs_getfs(p, &fs, &p2);
    if (ret < 0) {
        release_lock(&mount_lock);
        return ret;
    }

    if (fs->iops->lookup_all != NULL) {
        for (; *p2 == '/'; ++p2) {}
        int ret;
        if (strcmp(p2, "") == 0) {
            ret = fs->iops->getroot(fs, inode);
        } else {
            ret = fs->iops->lookup_all(fs, p2, inode);
        }
        if (ret < 0) {
            release_lock(&mount_lock);
            return ret;
        }
    } else {
        kassert(fs->iops->getroot != NULL);

        char path[PATH_MAX];
        strncpy(path, p2, PATH_MAX);

        fs->iops->getroot(fs, inode);

        char *save;
        for (char *i = strtok_r(path, "/", &save); i != NULL;
            i = strtok_r(NULL, "/", &save)) {

            int ret = fs->iops->lookup(inode, i, inode);
            if (ret < 0) {
                release_lock(&mount_lock);
                return ret;
            }
        }
    }

    release_lock(&mount_lock);
    return 0;
}

static void unmount_child(struct mount_tree *node) {
    if (node == NULL) {
        return;
    }

    unmount_child(node->child);
    unmount_child(node->sibling);
    kfree_sync(node);
}

int fs_mount(const char *targ, struct inode *src, const struct inode_ops *fs) {
    acquire_lock(&mount_lock);

    char path[PATH_MAX];
    strncpy(path, targ, PATH_MAX);

    char *save;

    struct mount_tree *node = &mount_root;
    for (char *i = strtok_r(path, "/", &save); i != NULL;
         i = strtok_r(NULL, "/", &save)) {


        bool insert = true;
        for (struct mount_tree *j = node->child; j != NULL; j = j->sibling) {
            if (strncmp(j->name, i, FILE_NAME_LEN) == 0) {
                node = j;
                insert = false;
            }
        }

        if (insert) {
            // allocate node
            struct mount_tree *nn = kmalloc_sync(sizeof(struct mount_tree));
            strncpy(nn->name, i, FILE_NAME_LEN);
            nn->mountpoint = false;
            nn->child = NULL;
            nn->sibling = node->child;

            node->child = nn;
            node = nn;
        }
    }

    unmount_child(node->child);
    node->mountpoint = true;
    fs_open(src, &(node->fs.file), 0);
    node->fs.iops = fs;

    release_lock(&mount_lock);
    return 0;
}

int fs_umount(const char *targ) {
    acquire_lock(&mount_lock);

    release_lock(&mount_lock);
    return 0;
}
