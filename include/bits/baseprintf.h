#ifndef BITS_BASEPRINTF_H
#define BITS_BASEPRINTF_H

#include <stdarg.h>

typedef int (*base_printf_putc_t)(int c, void *special);

int base_vprintf(base_printf_putc_t putfn, void *special,
                 const char *format, va_list ap);

#endif
