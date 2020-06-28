#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>
#include <sys/types.h>

enum seek_types {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);

int dup2(int fd, int fd2);

pid_t fork(void);

int execve(const char *path, char *const argv[], char *const envp[]);
int execvp(const char *path, char *const argv[]);

int pipe(int filedes[2]);
int pipe2(int filedes[2], int flags);

void _exit(int status);

int getopt(int argc, char * argv[], const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;


#endif
