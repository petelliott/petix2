#ifndef SYS_MMAN_H
#define SYS_MMAN_H

#include <sys/types.h>

#define MAP_FAILED ((void *) -1)

#define PROT_READ  0
#define PROT_WRITE 1
#define PROT_EXEC  2
#define PROT_NONE  3

#define MAP_SHARED  0
#define MAP_PRIVATE 1
#define MAP_FIXED   2

void *mmap(void *addr, size_t len, int prot, int flags,
           int fildes, off_t off);

#endif
