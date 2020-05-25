#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>

typedef size_t (*syscall_t)();

extern syscall_t syscall_table[256];

size_t sys_db_print(const char *str);


#endif
