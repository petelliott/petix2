#include <bits/baseprintf.h>
#include <stdint.h>

static void puts(base_printf_putc_t putfn, void *special,
                 const char *str) {

    for (; *str != 0; ++str) {
        putfn(*str, special);
    }
}

static void putuintnz(base_printf_putc_t putfn, void *special,
                      unsigned long num) {
    if (num != 0) {
        putuintnz(putfn, special, num/10);
        putfn((num % 10) + '0', special);
    }
}


static void putuint(base_printf_putc_t putfn, void *special,
                    unsigned long num) {
    if (num == 0) {
        putfn('0', special);
    } else {
        putuintnz(putfn, special, num);
    }
}

static void putint(base_printf_putc_t putfn, void *special,
                   long num) {
    if (num < 0) {
        putfn('-', special);
        putuint(putfn, special, -num);
    } else {
        putuint(putfn, special, num);
    }
}

static void puthex(base_printf_putc_t putfn, void *special,
                   unsigned long n) {
    putfn('0', special);
    putfn('x', special);
    for (int i = 2*sizeof(n) - 1; i >= 0; --i) {
        int dig = (n >> (i*4)) & 0xf;
        if (dig < 10) {
            putfn(dig + '0', special);
        } else {
            putfn((dig-10) + 'A', special);
        }
    }
}

int base_vprintf(base_printf_putc_t putfn, void *special,
                 const char *format, va_list ap) {

    for (const char *ch = format; *ch != 0; ++ch) {
        if (*ch == '%') {
            ++ch;
            if (*(ch) == 'l') {
                ++ch;
                if (*(ch) == 'l') {
                    ++ch;
                    switch (*ch) {
                    case 'd':
                    case 'i':
                        putint(putfn, special, va_arg(ap, long long int));
                        break;
                    case 'u':
                        putuint(putfn, special, va_arg(ap, long long unsigned int));
                        break;
                    case 'x':
                    case 'X':
                        puthex(putfn, special, va_arg(ap, long long unsigned int));
                        break;
                    }
                } else {
                    switch (*ch) {
                    case 'd':
                    case 'i':
                        putint(putfn, special, va_arg(ap, long int));
                        break;
                    case 'u':
                        putuint(putfn, special, va_arg(ap, long unsigned int));
                        break;
                    case 'x':
                    case 'X':
                        puthex(putfn, special, va_arg(ap, long unsigned int));
                        break;
                    }
                }

            } else {
                switch (*ch) {
                case '%':
                    putfn(*ch, special);
                    break;
                case 'd':
                case 'i':
                    putint(putfn, special, va_arg(ap, int));
                    break;
                case 'u':
                    putuint(putfn, special, va_arg(ap, unsigned int));
                    break;
                case 's':
                    puts(putfn, special, va_arg(ap, const char *));
                    break;
                case 'c':
                    putfn(va_arg(ap, int), special);
                    break;
                case 'p':
                    puthex(putfn, special, va_arg(ap, uintptr_t));
                    break;
                case 'x':
                case 'X':
                    puthex(putfn, special, va_arg(ap, unsigned int));
                    break;
                }
            }
        } else {
            putfn(*ch, special);
        }
    }
    //TODO
    return 0;
}
