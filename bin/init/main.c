#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>


int main() {
    char buff[50];
    int fd = raw_syscall(SYS_NR_OPEN, "/etc/motd");
    if (fd < 0) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(-fd));
    }

    ssize_t nr = raw_syscall(SYS_NR_READ, fd, buff, 50);
    if (nr < 0) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(-nr));
    }

    raw_syscall(SYS_NR_DB_PRINT, buff);



    size_t ret = raw_syscall(SYS_NR_FORK);
    if (ret == 0) {
        //while (1) {
        //    raw_syscall(SYS_NR_DB_PRINT, "A");
        //}
        return raw_syscall(SYS_NR_DB_PRINT, "hello from child");
    } else {
        //while (1) {
        //    raw_syscall(SYS_NR_DB_PRINT, "B");
        //}
        raw_syscall(SYS_NR_SCHED_YIELD);
        return raw_syscall(SYS_NR_DB_PRINT, "hello from parent");
    }
}
