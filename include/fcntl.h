#ifndef FCNTL_H
#define FCNTL_H

#define O_DIRECTORY (1 << 0)
#define O_CLOEXEC   (1 << 1)

int open(const char *path, int flags, ...);

#endif
