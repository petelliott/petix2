#include "kdebug.h"
#include "device/tty.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <bits/baseprintf.h>

static int kputc(int ch, void *spec) {
    char c = ch;
    tty_write(&c, 1);
    return 0;
}


void kprintf(const char *fmt, ...) {
    va_list ap;
	va_start(ap, fmt);
    base_vprintf(kputc, NULL, fmt, ap);
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
