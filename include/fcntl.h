#ifndef FCNTL_H
#define FCNTL_H

#define O_DIRECTORY (1 << 0)

int open(const char *path, int flags, ...);

#endif
