#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H

#include <stddef.h>
#include <sys/types.h>

#define SYSCALL_INT_NUM 0x80

enum syscall_nums {
    SYS_NR_READ  = 0,
    SYS_NR_WRITE = 1,
    SYS_NR_OPEN  = 2,
    SYS_NR_CLOSE = 3,
    SYS_NR_WAITPID = 4,
    SYS_NR_DUP2  = 5,
    SYS_NR_SCHED_YIELD = 24,
    SYS_NR_FORK     = 57,
    SYS_NR_EXEC     = 59,
    SYS_NR_EXIT     = 60,
    SYS_NR_DB_PRINT = 255
};

// call with whatever args the syscall takes
ssize_t raw_syscall(size_t sys_num, ...);

// sets errno and returns -1 if result is negative
ssize_t raw_syscall_errno(size_t sys_num, ...);


#endif
