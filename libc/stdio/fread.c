#include "file.h"

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!stream->valid) {
        stream->err = EBADF;
        return -1;
    }
    int ret = read(stream->fd, ptr, size*nmemb);
    if (ret == 0) {
        stream->eof = true;
        stream->err = 0;
    } else if (ret == -1) {
        stream->eof = true;
        stream->err = errno;
        return 0;
    }
    return ret;
}
