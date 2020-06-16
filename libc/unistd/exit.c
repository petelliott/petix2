#include <unistd.h>
#include <sys/syscall.h>


void _exit(int status) {
    raw_syscall_errno(SYS_NR_EXIT, status);
}
