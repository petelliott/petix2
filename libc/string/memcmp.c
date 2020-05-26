#include <string.h>

int memcmp(const void *s1, const void *s2, size_t n) {
    const char *ss1 = s1;
    const char *ss2 = s2;
    for (; *ss1 == *ss2 && n > 0; ++ss1, ++ss2, --n) {}
    return ((int) *((unsigned char *) ss1)) - *((unsigned char *) ss2);
}
