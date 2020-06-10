#ifndef DIRENT_H
#define DIRENT_H

#include <sys/types.h>

// raw syscall, non-posix
int getdent(int fd, struct petix_dirent *dent);

#endif
