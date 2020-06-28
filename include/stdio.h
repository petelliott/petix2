#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>

typedef struct _stdio_file FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define EOF -1
#define FOPEN_MAX 16

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define BUFSIZ 4096

FILE *fopen(const char *path, const char *mode);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fclose(FILE *stream);

int fflush(FILE *stream);

int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);

int fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);

int feof(FILE *stream);

int printf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
int fprintf(FILE *f, const char *format, ...)
    __attribute__ ((format (printf, 2, 3)));

int sprintf(char *str, const char *format, ...)
    __attribute__ ((format (printf, 2, 3)));
int snprintf(char *str, size_t size, const char *format, ...)
    __attribute__ ((format (printf, 3, 4)));

void perror(const char *s);

#endif
