#include "file.h"


int fflush(FILE *stream) {
    if (!stream->valid) {
        stream->err = EBADF;
        return EOF;
    }

    write(stream->fd, stream->buffer, stream->end);
    stream->end = 0;

    return 0;
}
