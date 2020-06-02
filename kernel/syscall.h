#ifndef SYSCALL_H
#define SYSCALL_H

#include <sys/types.h>
#include <stddef.h>

typedef ssize_t (*syscall_t)();

extern syscall_t syscall_table[256];

ssize_t sys_db_print(const char *str);

ssize_t sys_read(size_t fd, char *buf, size_t count);
ssize_t sys_write(size_t fd, const char *buf, size_t count);
ssize_t sys_open(const char *path, int flags, int mode);
ssize_t sys_close(size_t fd);

ssize_t sys_sched_yield(void);
ssize_t sys_fork(void);
ssize_t sys_exec(const char *path);
ssize_t sys_exit(size_t code);


#endif
