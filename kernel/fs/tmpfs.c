#include "tmpfs.h"
#include "../container/hashmap.h"
#include "../container/list.h"
#include "../kmalloc.h"
#include <string.h>
#include "../mem.h"
#include <errno.h>

struct tmpnode {
    struct inode inode;
    union {
        struct hashmap children;
        struct list pages;
    } contents;
};

static void free_tmpnode(void *node) {
    struct hashmap_entry *entry = node;

    char *key = entry->key;
    struct tmpnode *value = entry->value;

    kfree(key);

    if (value->inode.ftype == FT_DIR) {
        hashmap_free(&(value->contents.children));
    } else if (value->inode.ftype == FT_REGULAR) {
        list_foreach(char *, page, value->contents.pages) {
            free_page_ptr_sync(page);
        }
        list_free(&(value->contents.pages));
    }
    kfree(value);
    kfree(entry);
}

// inode operations

static int tmp_getroot(struct fs_inst *fs, struct inode *inode) {
    acquire_lock(&(fs->lock));
    if (fs->private_data == NULL) {
        struct tmpnode *node = kmalloc_sync(sizeof(struct tmpnode));
        node->inode.exec = false;
        node->inode.ftype = FT_DIR;
        node->inode.inode_id = (size_t) node;
        node->inode.fs = fs;

        hashmap_init(&(node->contents.children), string_key_hash, string_key_cmp, free_tmpnode);

        fs->private_data = node;
    }

    memcpy(inode, &(((struct tmpnode *)fs->private_data)->inode), sizeof(struct inode));

    release_lock(&(fs->lock));
    return 0;
}

static int tmp_lookup(struct inode *dir, const char *fname, struct inode *out) {
    kassert(dir->ftype == FT_DIR);
    if (dir->ftype != FT_DIR) {
        return -ENOTDIR;
    }
    acquire_lock(&(dir->fs->lock));

    struct tmpnode *parent = (struct tmpnode *)(dir->inode_id);

    struct tmpnode *node = hashmap_get(&(parent->contents.children), fname);

    int rc = 0;

    if (node == NULL) {
        rc = -ENOENT;
    } else {
        memcpy(out, &(node->inode), sizeof(struct inode));
    }

    release_lock(&(dir->fs->lock));
    return rc;
}

static int tmp_creat(struct inode *dir, const char *name) {
    if (dir->ftype != FT_DIR) {
        return -ENOTDIR;
    }
    acquire_lock(&(dir->fs->lock));

    struct tmpnode *parent = (struct tmpnode *)(dir->inode_id);

    struct tmpnode *node = kmalloc_sync(sizeof(struct tmpnode));
    node->inode.exec = false;
    node->inode.ftype = FT_REGULAR;
    node->inode.inode_id = (size_t) node;
    node->inode.fs = parent->inode.fs;
    node->inode.size = 0;

    list_init(&(node->contents.pages), sizeof(char *));

    int rc = 0;
    if (hashmap_get(&(parent->contents.children), name) != NULL) {
        rc = -EEXIST;
    } else {
        hashmap_insert(&(parent->contents.children), kstrdup_sync(name), node);
    }

    release_lock(&(dir->fs->lock));
    return rc;
}

static int tmp_mkdir(struct inode *dir, const char *name) {
    if (dir->ftype != FT_DIR) {
        return -ENOTDIR;
    }
    acquire_lock(&(dir->fs->lock));

    struct tmpnode *parent = (struct tmpnode *)(dir->inode_id);

    struct tmpnode *node = kmalloc_sync(sizeof(struct tmpnode));
    node->inode.exec = false;
    node->inode.ftype = FT_DIR;
    node->inode.inode_id = (size_t) node;
    node->inode.fs = parent->inode.fs;

    hashmap_init(&(node->contents.children), string_key_hash, string_key_cmp, free_tmpnode);

    int rc = 0;
    if (hashmap_get(&(parent->contents.children), name) != NULL) {
        rc = -EEXIST;
    } else {
        hashmap_insert(&(parent->contents.children), kstrdup_sync(name), node);
    }

    release_lock(&(dir->fs->lock));
    return rc;
}

// file operations

static int tmp_open(struct inode *inode, struct file *file, int mode) {
    file->size = inode->size;
    file->private_data = (off_t)inode->inode_id;
    return 0;
}

#define MIN(a, b) (((a)<(b))? (a):(b))
#define MAX(a, b) (((a)>(b))? (a):(b))

static ssize_t tmp_read(struct file *f, char *buf, size_t n) {
    kassert(f->inode.ftype == FT_REGULAR);
    acquire_lock(&(f->inode.fs->lock));

    size_t len = MIN(f->size - f->offset, n);

    struct tmpnode *node = (void *)f->inode.inode_id;

    for (size_t off = f->offset; off < (f->offset + len);) {
        size_t page = off / PAGE_SIZE;
        size_t poff = off % PAGE_SIZE;
        size_t delta = PAGE_SIZE - poff;

        char **data = list_at(&(node->contents.pages), page);

        memcpy(buf, (*data) + poff, PAGE_SIZE - poff);

        buf += delta;
        off += delta;
    }

    f->offset += len;

    release_lock(&(f->inode.fs->lock));
    return len;
}

#define CEIL(x, y) (((x) + (y) - 1)/(y))

static ssize_t tmp_write(struct file *f, const char *buf, size_t n) {
    kassert(f->inode.ftype == FT_REGULAR);
    acquire_lock(&(f->inode.fs->lock));

    struct tmpnode *node = (void *)f->inode.inode_id;

    // grow the file if necessary
    size_t curr_pages = CEIL(f->size, PAGE_SIZE);
    size_t needed_pages = CEIL(f->offset + n, PAGE_SIZE);

    for (size_t  i = 0; i < (needed_pages - curr_pages); ++i) {
        list_appendv((&node->contents.pages), alloc_page_ptr_sync());
    }

    f->size = MAX(f->size, f->offset + n);
    node->inode.size = f->size;

    // copy data over
    for (size_t off = f->offset; off < (f->offset + n);) {
        size_t page = off / PAGE_SIZE;
        size_t poff = off % PAGE_SIZE;
        size_t delta = MIN(PAGE_SIZE - poff, (f->offset + n) - poff);

        char **data = list_at(&(node->contents.pages), page);

        memcpy((*data) + poff, buf, PAGE_SIZE - poff);

        buf += delta;
        off += delta;
    }

    f->offset += n;

    release_lock(&(f->inode.fs->lock));
    return n;
}

static int tmp_getdent(struct file *f, struct petix_dirent *dirent) {
    kassert(f->inode.ftype == FT_DIR);
    acquire_lock(&(f->inode.fs->lock));

    struct tmpnode *node = (void *)f->inode.inode_id;

    void **start_iter = hashmap_begin(&(node->contents.children));
    struct hashmap_entry **entry = (void *)(start_iter + f->offset);

    if ((void **)entry >= hashmap_end(&(node->contents.children))) {
        dirent->present = false;
    } else {
        struct tmpnode *child = (*entry)->value;
        dirent->inode_id = child->inode.inode_id;
        dirent->present = true;
        dirent->type = child->inode.ftype;
        strncpy(dirent->name, (char *) (*entry)->key, FILE_NAME_LEN);

        f->offset = hashmap_next((void **)entry) - start_iter;
    }

    release_lock(&(f->inode.fs->lock));
    return 0;
}

const struct inode_ops *get_tmpfs(void) {
    static struct inode_ops iops;
    iops = (struct inode_ops) {
        .reg_ops = {
            .open = tmp_open,
            .lseek = fs_default_lseek,
            .read = tmp_read,
            .write = tmp_write,
            .getdent = tmp_getdent
        },
        .creat = tmp_creat,
        .mkdir = tmp_mkdir,
        .getroot = tmp_getroot,
        .lookup = tmp_lookup
    };
    return &iops;
}
