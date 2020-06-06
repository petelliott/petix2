#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>

typedef struct _stdio_file FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fclose(FILE *stream);

int fputc(int c, FILE *stream);

int printf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
int fprintf(FILE *f, const char *format, ...)
    __attribute__ ((format (printf, 2, 3)));




#endif
