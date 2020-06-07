#include <string.h>

char *strcat(char * restrict s1,
             const char * restrict s2);

char *strncat(char * restrict dest,
              const char * restrict src,
              size_t n) {

    size_t off = strnlen(dest, n);
    strncpy(dest+off, src, n-off);

    return dest;
}
