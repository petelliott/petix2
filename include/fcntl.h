#ifndef FCNTL_H
#define FCNTL_H

#include <sys/types.h>

#define O_DIRECTORY (1 << 0)
#define O_CLOEXEC   (1 << 1)

int open(const char *path, int flags, ...);

int creat(const char *pathname, mode_t mode);

#endif
