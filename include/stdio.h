#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>

typedef struct _stdio_file FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define EOF -1

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fclose(FILE *stream);

int fputc(int c, FILE *stream);

int fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);

int feof(FILE *stream);

int printf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
int fprintf(FILE *f, const char *format, ...)
    __attribute__ ((format (printf, 2, 3)));




#endif
