#ifndef TTY_H
#define TTY_H

#include <stddef.h>

/*
void term_clear(void);
void term_putchar(char c, enum vga_color fg, enum vga_color bg);
void term_writes(const char *str, enum vga_color fg, enum vga_color bg);

void print(const char *str);
void warn(const char *str);
void error(const char *str);

void printx(unsigned long n);
*/

void tty_init(void);

size_t tty_write(const void *buf, size_t count);

size_t tty_read(void *buf, size_t count);

#endif
