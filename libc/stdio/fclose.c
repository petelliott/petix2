#include "file.h"

int fclose(FILE *stream) {
    if (!stream->valid) {
        stream->err = EBADF;
        return -1;
    }
    int ret = close(stream->fd);
    if (ret == -1) {
        stream->err = errno;
    }
    return ret;
}
