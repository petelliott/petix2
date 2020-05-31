#include <string.h>

int strcmp(const char *s1, const char *s2) {
    for (; *s1 == *s2 && *s1 != 0; ++s1, ++s2) {}
    return ((int) *((unsigned char *) s1)) - *((unsigned char *) s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
    for (; *s1 == *s2 && *s1 != 0 && n !=0; ++s1, ++s2, --n) {}
    if (n == 0) {
        return 0;
    } else {
        return ((int) *((unsigned char *) s1)) - *((unsigned char *) s2);
    }
}
