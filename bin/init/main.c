#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


int main() {

    int ttyfd = open("/dev/tty", 0);
    if (ttyfd == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
    }

    if (write(ttyfd, "hey tty\n", 8) == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
    }
    if (write(ttyfd, "hey tty\n", 8) == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
    }

    if (close(ttyfd) == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
    }

    int ttyfd2 = open("/dev/tty", 0);
    if (ttyfd2 == ttyfd) {
        raw_syscall(SYS_NR_DB_PRINT, "close works");
    }




    char buff[50];
    int fd = open("/etc/motd", 0);
    if (fd == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
    }

    ssize_t nr = write(fd, "aaaa", 4);
    if (nr == -1) {
        raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
    }

    pid_t ret = fork();
    if (ret == 0) {
        ssize_t nr = read(fd, buff, 50);
        if (nr == -1) {
            raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
        }

        raw_syscall(SYS_NR_DB_PRINT, buff);
        //while (1) {
        //    raw_syscall(SYS_NR_DB_PRINT, "A");
        //}
        return raw_syscall(SYS_NR_DB_PRINT, "hello from child");
    } else {
        ssize_t nr = read(fd, buff, 50);
        if (nr == -1) {
            raw_syscall(SYS_NR_DB_PRINT, strerror(errno));
        }

        raw_syscall(SYS_NR_DB_PRINT, buff);
        //while (1) {
        //    raw_syscall(SYS_NR_DB_PRINT, "B");
        //}
        raw_syscall(SYS_NR_SCHED_YIELD);
        return raw_syscall(SYS_NR_DB_PRINT, "hello from parent");
    }
}
