#include "file.h"

FILE stdio_files[FOPEN_MAX] = {
    { .fd = STDIN_FILENO, .err = 0, .bf = _IOLBF, .start = 0,
      .end = 0, .valid = true },
    { .fd = STDOUT_FILENO, .err = 0, .bf = _IOLBF, .start = 0,
      .end = 0, .valid = true },
    { .fd = STDERR_FILENO, .err = 0, .bf = _IONBF, .start = 0,
      .end = 0, .valid = true },
};

FILE *stdin = &(stdio_files[0]);
FILE *stdout = &(stdio_files[1]);
FILE *stderr = &(stdio_files[2]);
