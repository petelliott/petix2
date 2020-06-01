#include "kdebug.h"
#include "device/tty.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

static void kputc(char ch) {
    tty_write(&ch, 1);
}

static void kputs(const char *str) {
    tty_write(str, strlen(str));
}

static void putuintnz(unsigned long num) {
    if (num != 0) {
        putuintnz(num/10);
        kputc((num % 10) + '0');
    }
}

static void putuint(unsigned long num) {
    if (num == 0) {
        kputc('0');
    } else {
        putuintnz(num);
    }
}

static void putint(long num) {
    if (num < 0) {
        kputc('-');
        putuint(-num);
    } else {
        putuint(num);
    }
}

static void puthex(unsigned long n) {
    kputs("0x");
    for (int i = 2*sizeof(n) - 1; i >= 0; --i) {
        int dig = (n >> (i*4)) & 0xf;
        if (dig < 10) {
            kputc(dig + '0');
        } else {
            kputc((dig-10) + 'A');
        }
    }
}

void kprintf(const char *fmt, ...) {
    va_list ap;
	va_start(ap, fmt);

    for (const char *ch = fmt; *ch != 0; ++ch) {
        if (*ch == '%') {
            ++ch;
            if (*(ch) == 'l') {
                ++ch;
                if (*(ch) == 'l') {
                    ++ch;
                    switch (*ch) {
                    case 'd':
                    case 'i':
                        putint(va_arg(ap, long long int));
                        break;
                    case 'u':
                        putuint(va_arg(ap, long long unsigned int));
                        break;
                    case 'x':
                    case 'X':
                        puthex(va_arg(ap, long long unsigned int));
                        break;
                    }
                } else {
                    switch (*ch) {
                    case 'd':
                    case 'i':
                        putint(va_arg(ap, long int));
                        break;
                    case 'u':
                        putuint(va_arg(ap, long unsigned int));
                        break;
                    case 'x':
                    case 'X':
                        puthex(va_arg(ap, long unsigned int));
                        break;
                    }
                }

            } else {
                switch (*ch) {
                case '%':
                    kputc(*ch);
                    break;
                case 'd':
                case 'i':
                    putint(va_arg(ap, int));
                    break;
                case 'u':
                    putuint(va_arg(ap, unsigned int));
                    break;
                case 's':
                    kputs(va_arg(ap, const char *));
                    break;
                case 'c':
                    kputc(va_arg(ap, int));
                    break;
                case 'p':
                    puthex(va_arg(ap, uintptr_t));
                    break;
                case 'x':
                case 'X':
                    puthex(va_arg(ap, unsigned int));
                    break;
                }
            }
        } else {
            kputc(*ch);
        }
    }
    va_end(ap);
}

// TODO: portability
static void halt() {
    asm("cli");
    while (1) {
        asm("hlt");
    }
}

void kassert_internal(int tst, int line, const char *file, const char *tststr) {
    if (!tst) {
        kprintf("kassert failed (%s:%i): %s\n", file, line, tststr);
        halt();
    }
}

void panic_internal(const char *str, int line, const char *file) {
    kprintf("kernel panic (%s:%i): %s\n", file, line, str);
    halt();
}
