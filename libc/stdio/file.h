#ifndef STDIO_FILE_H
#define STDIO_FILE_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

struct _stdio_file {
    int fd;
    int err;
    bool valid;
    bool eof;
};

#define MAX_FILES 16

extern FILE stdio_files[MAX_FILES];

#endif
