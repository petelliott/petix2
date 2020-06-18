#include "ansiseq.h"
#include "../../kdebug.h"
#include <string.h>


static void clear(struct ansi_term *term) {
    for (int r = 0; r < term->backend->row_n; ++r) {
        for (int c = 0; c < term->backend->col_n; ++c) {
            term->backend->putch(&(term->rendition), ' ', r, c);
        }
    }
}


void ansi_init(struct ansi_term *term) {
    const struct ansi_backend *b = term->backend;
    memset(term, 0, sizeof(term));
    term->backend = b;

    term->state = ANSI_S_NONE;

    term->rendition.fg = ANSI_DEFAULT;
    term->rendition.bg = ANSI_DEFAULT;

    clear(term);
}

static void putch_unesc(struct ansi_term *term, char ch) {
    if (ch == '\n') {
        ++term->row;
        term->col = 0;
    } else if (ch == '\r') {
        term->col = 0;
    } else if (ch == '\t') {
        term->col += 8;
    } else if (ch == '\b') {
        if (--term->col == -1) {
            term->col = term->backend->col_n - 1;
            --term->row;
        }
        term->backend->putch(&(term->rendition), ' ', term->row, term->col);
    } else {
        term->backend->putch(&(term->rendition), ch, term->row, term->col);
        if (++term->col == term->backend->col_n) {
            term->col = 0;
            ++term->row;
        }
    }
    if (term->row == term->backend->row_n) {
        term->backend->scroll_up();
        term->row--;
    }
    term->backend->curto(term->row, term->col);
}

static void process_rendition_command(struct ansi_term *term, int nargs) {
    for (size_t i = 0; i < nargs; ++i) {
        int cmd = term->numerics[i];
        if (cmd/10 == 3) {
            term->rendition.fg = cmd % 10;
        } else if (cmd/10 == 4) {
            term->rendition.bg = cmd % 10;
        } else if (cmd == 0) {
            term->rendition.flags.bits = 0;
            term->rendition.fg = ANSI_DEFAULT;
            term->rendition.bg = ANSI_DEFAULT;
        } else if (cmd/10 == 0) {
            term->rendition.flags.bits |= (1 << (cmd %10));
        } else if (cmd/10 == 2) {
            if (cmd % 10 == 2) {
                term->rendition.flags.f.bold = false;
                term->rendition.flags.f.faint = false;
            } else {
                term->rendition.flags.bits &= ~(1 << (cmd %10));
            }
        }
    }
}

void ansi_putch(struct ansi_term *term, char ch) {
    if (term->state == ANSI_S_NONE && ch == '\e') {
        term->noff = 0;
        term->nump = false;
        term->numerics[term->noff] = 0;

        term->state = ANSI_S_ESC;
        return;
    }

    if (term->state == ANSI_S_ESC && ch == '[') {
        term->state = ANSI_S_ESC_LBRAK;
        return;
    }

    if ((term->state == ANSI_S_ESC || term->state == ANSI_S_ESC_LBRAK)
        && ch >= '0' && ch <= '9') {

        term->nump = true;

        term->numerics[term->noff] *= 10;
        term->numerics[term->noff] += ch - '0';
        return;
    }

    if ((term->state == ANSI_S_ESC || term->state == ANSI_S_ESC_LBRAK)
        && ch == ';') {

        term->noff++;
        term->numerics[term->noff] = 0;
        return;
    }
    //now we are at the end of a command or are writing something

    size_t nargs =0;
    if (term->nump) {
        nargs = term->noff + 1;
    }

    if (term->state == ANSI_S_NONE) {
        putch_unesc(term, ch);
    } else if (term->state == ANSI_S_ESC) {
        term->state = ANSI_S_NONE;
    } else if (term->state == ANSI_S_ESC_LBRAK) {
        if (ch == 'm') {
            process_rendition_command(term, nargs);
        }
        term->state = ANSI_S_NONE;
    } else {
        panic("unsupported ansi terminal state");
    }
}
