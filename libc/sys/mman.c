#include <sys/mman.h>
#include <sys/syscall.h>
#include <errno.h>

void *mmap(void *addr, size_t len, int prot, int flags,
           int fildes, off_t off) {

    return (void *) raw_syscall(SYS_NR_MMAP, addr, len, prot, flags,
                                fildes, off, &errno);
}
