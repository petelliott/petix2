#include <string.h>

void *memset(void *s, int c, size_t n) {
    char *cs = s;
    for (size_t i = 0; i < n; ++i) {
        cs[i] = c;
    }
    return s;
}
