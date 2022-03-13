#ifndef KDEBUG_H
#define KDEBUG_H

#define kassert(tst) kassert_internal((int)(tst), __LINE__, __FILE__, #tst)
#define panic(str) panic_internal((str), __LINE__, __FILE__)

void kprintf(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));

void kassert_internal(int tst, int line, const char *file, const char *tststr);
void panic_internal(const char *str, int line, const char *file);

#endif
