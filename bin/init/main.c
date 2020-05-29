#include <sys/syscall.h>

int main() {
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
