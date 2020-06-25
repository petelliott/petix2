#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>


static struct termios save;
static struct winsize wsize;

static void enable_raw(void) {
    // we use stdout as the terminal, because stdin can be piped to
    if (tcgetattr(STDOUT_FILENO, &save) == -1) {
        perror("tcgetattr(3)");
    }

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize) == -1) {
        perror("ioctl(2)");
    }

    struct termios raw = save;
    raw.c_lflag &= ~(ECHO | ICANON);

    if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr(3)");
    }
}

static void disable_raw(void) {
    if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, &save) == -1) {
        perror("tcsetattr(3)");
    }
}

static char getkey(void) {
    char buff[8];
    read(STDOUT_FILENO, buff, sizeof(buff));
    return buff[0];
};

int main(int argc, char *argv[]) {

    FILE *in = stdin;
    if (argc > 1) {
        in  = fopen(argv[1], "r");
    }

    enable_raw();

    char buff[1024];

    for (size_t i = 0; i < wsize.ws_row - 1; ++i) {
        fgets(buff, sizeof(buff), in);
        if (feof(in)) {
            break;
        }
        fputs(buff, stdout);
    }

    while (!feof(in)) {
        fputs("\e[7m--MORE--\e[0m", stdout);
        fflush(stdout);

        char ch = getkey();
        fputs("\r        \r", stdout); // overwrite "--MORE--"
        fflush(stdout);

        if (ch == '\n') {
            fgets(buff, sizeof(buff), in);
            fputs(buff, stdout);
        } else if (ch == ' ') {
            for (size_t i = 0; i < wsize.ws_row-1; ++i) {
                //TODO no reason to write
                fgets(buff, sizeof(buff), in);
                fputs(buff, stdout);
            }
        } else if (ch == 'q' || ch == 0x03) {
            break;
        }
    }

    disable_raw();
    return 0;
}
