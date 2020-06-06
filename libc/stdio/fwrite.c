#include "file.h"

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!stream->valid) {
        stream->err = EBADF;
        return -1;
    }
    int ret = write(stream->fd, ptr, size*nmemb);
    if (ret == -1) {
        stream->err = errno;
    }
    return ret;
}
