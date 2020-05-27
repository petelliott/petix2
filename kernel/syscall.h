#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>

typedef size_t (*syscall_t)();

extern syscall_t syscall_table[256];

size_t sys_db_print(const char *str);

size_t sys_sched_yield(void);
size_t sys_fork(void);
size_t sys_exec(const char *path);
size_t sys_exit(size_t code);


#endif
