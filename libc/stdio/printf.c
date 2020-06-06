#include "file.h"
#include <stdarg.h>
#include <bits/baseprintf.h>

int printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = base_vprintf((base_printf_putc_t)fputc, stdout, format, ap);
    va_end(ap);
    return ret;
}

int fprintf(FILE *f, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = base_vprintf((base_printf_putc_t)fputc, f, format, ap);
    va_end(ap);
    return ret;
}
