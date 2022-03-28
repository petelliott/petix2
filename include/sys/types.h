#ifndef PETIX2_SYS_TYPES_H
#define PETIX2_SYS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef long ssize_t;
typedef long long off_t;

typedef ssize_t pid_t;

typedef uint32_t dev_t;
#define MAJOR(dev) (((dev) >> 16) & 0xffff)
#define MINOR(dev) ((dev) & 0xffff)
#define MKDEV(ma, mi) (((ma) << 16) | (mi))

#define FILE_NAME_LEN 256

struct petix_dirent {
    size_t inode_id;
    bool present;
    enum {
        DT_DIR,
        DT_LINK,
        DT_SPECIAL,
        DT_REGULAR
    } type;
    char name[FILE_NAME_LEN];
};

typedef uint32_t mode_t;

#endif
