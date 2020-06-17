#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <stdarg.h>

int ioctl(int fd, unsigned long request, ...) {
    va_list v;
    va_start(v, request);

    // syscalls can take at most 5 arguments
    // C does not specify evaluation order
    size_t a = va_arg(v, size_t);
    size_t b = va_arg(v, size_t);
    size_t c = va_arg(v, size_t);
    size_t d = va_arg(v, size_t);
    size_t e = va_arg(v, size_t);

    return raw_syscall_errno(SYS_NR_IOCTL, a, b, c, d, e);
}
