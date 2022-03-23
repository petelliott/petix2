#include "file.h"

int fclose(FILE *stream) {
    if (!stream->valid) {
        stream->err = EBADF;
        return -1;
    }
    fflush(stream);
    int ret = close(stream->fd);
    stream->valid = 0;
    if (ret == -1) {
        stream->err = errno;
    }
    return ret;
}
