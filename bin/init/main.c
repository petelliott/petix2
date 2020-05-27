#include <sys/syscall.h>

int main() {
    size_t ret = raw_syscall(SYS_NR_FORK);
    if (ret == 0) {
        return raw_syscall(SYS_NR_DB_PRINT, "hello from child");
    } else {
        raw_syscall(SYS_NR_SCHED_YIELD);
        return raw_syscall(SYS_NR_DB_PRINT, "hello from parent");
    }
}
