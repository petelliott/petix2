#include <stdio.h>
#include <bits/baseprintf.h>

static int sputc(int c, void *special) {
    char **cs = (char **) special;
    **cs = c;
    (*cs)++;
    return 0;
}

int sprintf(char *str, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = base_vprintf(sputc, &str, format, ap);
    va_end(ap);
    return ret;
}

/*
int snprintf(char *str, size_t size, const char *format, ...) {

}
*/
