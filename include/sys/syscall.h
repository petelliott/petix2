#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H

#include <stddef.h>

#define SYSCALL_INT_NUM 0x80

enum syscall_nums {
    SYS_NR_FORK     = 57,
    SYS_NR_EXEC     = 59,
    SYS_NR_EXIT     = 60,
    SYS_NR_DB_PRINT = 255
};

// call with whatever args the syscall takes
size_t raw_syscall(size_t sys_num, ...);

#endif
