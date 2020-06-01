#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


int main() {
    char buff[50];
    int fd = open("/etc/motd", 0);
    if (fd == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
    }

    ssize_t nr = read(fd, buff, 50);
    if (nr == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
    }

    raw_syscall(SYS_NR_DB_PRINT, buff);



    pid_t ret = fork();
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
