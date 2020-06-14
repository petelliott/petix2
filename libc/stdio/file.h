#ifndef STDIO_FILE_H
#define STDIO_FILE_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

struct _stdio_file {
    int fd;
    int err;
    int bf;
    bool valid;
    bool eof;
    size_t start, end;
    char buffer[BUFSIZ];
};

extern FILE stdio_files[FOPEN_MAX];

#endif
