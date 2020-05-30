#include <string.h>

char *strtok(char * restrict s1,
             const char * restrict s2) {
    static char *save;
    return strtok_r(s1,s2, &save);
}

char *strtok_r(char * restrict str,
               const char * restrict delim,
               char * restrict *save) {

    if (str == NULL) {
        str = *save;
    }


    for (; *str != 0 && strchr(delim, *str) != NULL; ++str) {}

    if (*str == 0) {
        return NULL;
    }
    char * ret = str;

    for (; *str != 0 && strchr(delim, *str) == NULL; ++str) {}
    *str = '\0';
    *save = str+1;

    return ret;
}
