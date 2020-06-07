#include <string.h>

size_t strlen(const char *s) {
    size_t len;
    for (len = 0; s[len]; ++len) {}
    return len;
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t len;
    for (len = 0; len <= maxlen && s[len]; ++len) {}
    return len;
}
