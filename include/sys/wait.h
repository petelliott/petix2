#ifndef SYS_WAIT_H
#define SYS_WAIT_H

#include <sys/types.h>

pid_t wait(int *wstatus);

pid_t waitpid(pid_t pid, int *wstatus, int options);

#endif
