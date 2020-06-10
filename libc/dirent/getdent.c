#include <dirent.h>
#include <sys/syscall.h>

int getdent(int fd, struct petix_dirent *dent) {
    return raw_syscall_errno(SYS_NR_GETDENT, fd, dent);
}
