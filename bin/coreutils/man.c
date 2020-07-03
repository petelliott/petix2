#include <stdlib.h>
#include <stdio.h>

// TODO a real implementation
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "useage: %s n keyword\n", argv[0]);
        return 1;
    }
    char buff[1024];

    sprintf(buff, "md /man/man%s/%s.md | more", argv[1], argv[2]);
    system(buff);
    return 0;
}
