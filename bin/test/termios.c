#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main() {
    if (ioctl(STDIN_FILENO, TCGETS) == -1 && errno == ENOTTY) {
        printf("ioctl works\n");
    } else {
        printf("ioctl doesn't work\n");
    }
    return 0;
}
