#include "syscall.h"
#include "kdebug.h"
#include <sys/syscall.h>

syscall_t syscall_table[256] = {
    [SYS_NR_DB_PRINT] = sys_db_print
};

size_t sys_db_print(const char *str) {
    kprintf("sys_db_print: \"%s\"\n", str);
    return 4;
}
