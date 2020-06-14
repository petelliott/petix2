#include "file.h"
#include <string.h>

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!stream->valid) {
        stream->err = EBADF;
        return -1;
    }

    if (stream->bf == _IONBF) {

        int ret = write(stream->fd, ptr, size*nmemb);
        if (ret == -1) {
            stream->err = errno;
            return 0;
        }
        return ret;

    } else {

        bool flushnl = (stream->bf == _IOLBF)
            && (memchr(ptr, '\n', size*nmemb) != NULL);

        if (size*nmemb + stream->end <= BUFSIZ && !flushnl) {
            memcpy(stream->buffer + stream->end, ptr, size*nmemb);
            stream->end += size*nmemb;
            return size*nmemb;
        } else {
            int r1 = write(stream->fd, stream->buffer, stream->end);
            if (r1 == -1) {
                stream->err = r1;
                return 0;
            }

            stream->end = 0;

            int r2 = write(stream->fd, ptr, size*nmemb);
            if (r2 == -1) {
                stream->err = r2;
                return r1;
            }

            return r1+r2;
        }

    }
}
