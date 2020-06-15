#include "file.h"
#include <string.h>

#define MIN(a, b) ((a < b)? a:b)

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!stream->valid) {
        stream->err = EBADF;
        return -1;
    }

    ssize_t ret = 0;
    if (stream->bf == _IONBF) {
        ret = read(stream->fd, ptr, size*nmemb);
    } else {
        char *cptr = ptr;
        size_t total = size*nmemb;

        while (total > 0) {
            size_t bufread = MIN(stream->end - stream->start, total);
            memcpy(cptr, stream->buffer+stream->start, bufread);
            total -= bufread;
            stream->start += bufread;
            cptr += bufread;

            if (total > BUFSIZ) {
                int r = read(stream->fd, cptr, total);
                if (r == -1) {
                    ret = -1;
                    break;
                }
                total -= r;
                break;
            } else if (total > 0) {
                stream->start = 0;
                int r = read(stream->fd, stream->buffer, BUFSIZ);
                if (r == -1) {
                    ret = -1;
                    break;
                } else if (r == 0) {
                    break;
                }
                stream->end = r;
            }
        }
        if (ret != -1) {
            ret = size*nmemb-total;
        }
    }

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
