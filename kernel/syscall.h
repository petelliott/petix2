#ifndef SYSCALL_H
#define SYSCALL_H

#include <sys/types.h>
#include <stddef.h>

typedef ssize_t (*syscall_t)();

extern syscall_t syscall_table[256];

ssize_t sys_db_print(const char *str);

ssize_t sys_read(ssize_t fd, char *buf, size_t count);
ssize_t sys_write(ssize_t fd, const char *buf, size_t count);
ssize_t sys_open(const char *path, int flags, int mode);
ssize_t sys_close(ssize_t fd);

ssize_t sys_dup2(ssize_t fd, ssize_t fd2);

ssize_t sys_getdent(ssize_t fd, struct petix_dirent *dent);
ssize_t sys_creat(const char *path);
ssize_t sys_mkdir(const char *path);

ssize_t sys_pipe(int filedes[2], size_t flags);

ssize_t sys_waitpid(pid_t pid, int *wstatus, int options);

ssize_t sys_ioctl(ssize_t fd, size_t req, ...);

void *sys_mmap(void *addr, size_t len, size_t prot, size_t flags,
               int filedes, off_t off, int *errno);

ssize_t sys_sched_yield(void);
ssize_t sys_fork(void);
ssize_t sys_exec(const char *path, char *const argv[], char *const envp[]);
ssize_t sys_exit(size_t code);


#endif
