#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


int main(int argc, char *argv) {
    if (argc == 1) {
        int fdcl = open("/bin/test/cloexec", O_CLOEXEC);
        if (fdcl != 3) {
            fprintf(stderr, "cloexec test assumption failed, fix the test\n");
            return 1;
        }
        int fdncl = open("/bin/test/cloexec", 0);
        if (fdncl != 4) {
            fprintf(stderr, "cloexec test assumption failed, fix the test\n");
            return 1;
        }

        char *const nargv[] = {"/bin/test/cloexec", "", NULL};
        execve("/bin/test/cloexec", nargv, NULL);
        perror("execve(2)");

        return 1;
    } if (argc == 2) {
        int ret;

        ret = read(3, NULL, 0);
        if (ret != -1 || errno != EBADF) {
            fprintf(stderr, "FAIL: cloexec'd fd not closed\n");
            return 1;
        }

        ret = read(4, NULL, 0);
        if (ret == -1) {
            perror("read(2)");
            fprintf(stderr, "FAIL: non-cloexec'd fd closed\n");
            return 1;
        }

        printf("cloexec test passed!\n");
    }

    return 0;
}
