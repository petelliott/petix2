#include <sys/syscall.h>
#include <errno.h>
#include <stdarg.h>

ssize_t raw_syscall_errno(size_t sys_num, ...) {
    va_list v;
    va_start(v, sys_num);

    // syscalls can take at most 5 arguments
    // C does not specify evaluation order
    size_t a = va_arg(v, size_t);
    size_t b = va_arg(v, size_t);
    size_t c = va_arg(v, size_t);
    size_t d = va_arg(v, size_t);
    size_t e = va_arg(v, size_t);

    ssize_t res = raw_syscall(sys_num, a, b, c, d, e);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}
