#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // fake shell

    char buff[1024];
    while (!feof(stdin)) {
        printf("$ ");
        fgets(buff, sizeof(buff), stdin);
        printf(buff);
    }

    return 0;
}
