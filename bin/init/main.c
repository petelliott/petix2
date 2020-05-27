#include <sys/syscall.h>

int main() {
    raw_syscall(SYS_NR_FORK);
    return raw_syscall(SYS_NR_DB_PRINT, "hello world");
}
