#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <string.h>


int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "usage: %s dir\n", argv[0]);
        return 1;
    }

    const char *dir = "/";
    if (argc == 2) {
        dir = argv[1];
    }

    int fd = open(dir, O_DIRECTORY);
    if (fd == -1) {
        perror("open(2)");
        return 1;
    }

    struct petix_dirent dent;

    while (1) {
        int ret = getdent(fd, &dent);
        if (ret == -1) {
            perror("getdent(2)");
        }
        if (!dent.present) break;

        if (dent.type == DT_DIR) {
            printf("%s/\n", dent.name);
        } else {
            printf("%s\n", dent.name);
        }
    }

    close(fd);

    return 0;
}
