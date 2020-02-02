#include <string.h>

int strcmp(const char *s1, const char *s2) {
    for (; *s1 == *s2 && *s1 != 0; ++s1, ++s2) {}
    return ((int) *((unsigned char *) s1)) - *((unsigned char *) s2);
}
