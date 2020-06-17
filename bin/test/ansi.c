#include <stdio.h>


int main() {
    printf("\e[39mcolour \e[7mnegative\e[27;1m bold\e[22;5m blink\e[0m\n");

    for (size_t i = 0; i < 7; ++i) {
        printf("\e[3%lumcolour \e[7mnegative\e[27;1m bold\e[22;5m blink\e[0m\n", i);
    }
    return 0;
}
