#include <string.h>

void *memchr(const void *s, int c, size_t n) {
    const char *cs = s;
    for (size_t i = 0; i < n; ++i) {
        if (cs[i] == c) {
            return (void *) cs+i;
        }
    }
    return NULL;
}
