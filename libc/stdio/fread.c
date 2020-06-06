#include "file.h"

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!stream->valid) {
        stream->err = EBADF;
        return -1;
    }
    int ret = read(stream->fd, ptr, size*nmemb);
    if (ret == -1) {
        stream->err = errno;
    }
    return ret;
}
