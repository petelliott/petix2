#include "kbd.h"
#include "../../arch/cpu.h"
#include "../../kdebug.h"

static bool initialized = false;

static void onkeypress(int scancode);

#define MAX_TTYS 16

static struct petix_tty *ttys[MAX_TTYS];
static int curtty = 0;

void register_kbd_tty(struct petix_tty *tty) {
    if (!initialized) {
        register_keypress(onkeypress);
        initialized = true;
    }
    kassert(curtty != MAX_TTYS);
    ttys[curtty++] = tty;
}

// stolen from serenityos
static const char en_map[0x80] = {
    0, '\033', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
    '=', 0x08, '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '\\', 0, 0, 0,
};

static const char en_shift_map[0x80] = {
    0, '\033', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
    '+', 0x08, '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
    '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '|', 0, 0, 0,
};


static bool shifted = false;
static bool ctrld   = false;

// returns -1 if invalid
static char sc_to_ascii(int scancode) {
    if (ctrld) {
        char ch = en_map[scancode];
        if (ch == 0) {
            return -1;
        }

        if (ch >= 'a') {
            return ch - 0x60;
        } else {
            return 0;
        }
    }

    char ret;
    if (shifted) {
        ret = en_shift_map[scancode];
    } else {
        ret = en_map[scancode];
    }

    if (ret == 0) {
        return -1;
    }
    return ret;
}

// interrupt handler
static void onkeypress(int scancode) {
    if (scancode == 0x36 || scancode == 0x2a) {
        shifted = true;
    } else if (scancode == 0xb6 || scancode == 0xaa) {
        shifted = false;
    } else if (scancode == 0x1d) {
        ctrld = true;
    } else if (scancode == 0x9d) {
        ctrld = false;
    } else if (scancode < 0x80) {
        char ch = sc_to_ascii(scancode);
        if (ch != -1) {
            for (size_t i = 0; i < curtty; ++i) {
                petix_tty_input_seq(ttys[i], &ch, 1);
            }
        }
    }
}
