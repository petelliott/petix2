#include "read.h"
#include <stddef.h>
#include <string.h>

char *parse_line(char *line, char **args) {
    for (; *line == ' ' || *line == '\t'; ++line) {}
    if (*line == '#') {
        return NULL;
    }

    char *save;

    size_t i = 0;
    for (char *c = strtok_r(line, " \t\n", &save); c != NULL;
         c = strtok_r(NULL, " \t\n", &save)) {

        args[i++] = c;
    }
    args[i] = NULL;

    return args[0];
}
