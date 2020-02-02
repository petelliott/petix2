#include <string.h>

void *memcpy(void * restrict s1,
             const void * restrict s2,
             size_t n) {
    char *cs1 = s1;
    const char *cs2 = s2;
    for (size_t i = 0; i < n; ++i) {
        cs1[i] = cs2[i];
    }
    return s1;
}
