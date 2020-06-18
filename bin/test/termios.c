#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>

static const char *const print_map[128] = {
    "^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "\\b", "\\t",
    "\\n", "^K", "^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S", "^T",
    "^U", "^V", "^W", "^X", "^Y", "^Z", "^[", "^\\", "^]", "^^", "^_",
    " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",",
    "-", ".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    ":", ";", "<", "=", ">", "?", "@", "A", "B", "C", "D", "E", "F",
    "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S",
    "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_", "`",
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "{", "|", "}", "~", "^?"
};

int main() {
    struct termios orig, raw;

    if (tcgetattr(STDIN_FILENO, &orig) == -1) {
        perror("tcgetattr(3)");
        return 1;
    }

    if (orig.c_lflag == (ICANON | ECHO | ISIG)) {
        printf("tcgetattr works\n");
    } else {
        printf("tcgetattr doesn't work\n");
        return 0;
    }
    raw = orig;

    raw.c_lflag &= ~(ECHO | ICANON);

    printf("entering raw mode...\n");
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr(3)");
        return 1;
    }

    printf("in raw mode, q to quit\n");

    while (1) {
        char buff[24];
        int r = read(STDIN_FILENO, buff, sizeof(buff));
        if (r == -1) {
            perror("read(2)");
            return 1;
        }
        printf("\"");
        for (size_t i = 0; i < r; ++i) {
            printf("%s", print_map[(int)buff[i]]);
        }
        printf("\"\n");

        if (buff[0] == 'q') {
            break;
        }
    }

    printf("entering cooked mode...\n");
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig) == -1) {
        perror("tcsetattr(3)");
        return 1;
    }

    printf("in cooked mode\n");

    return 0;
}
