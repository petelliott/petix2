#include <sys/syscall.h>

int main() {
    return raw_syscall(SYS_NR_DB_PRINT, "hello world");
}
