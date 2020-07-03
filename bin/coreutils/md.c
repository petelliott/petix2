/* an ansi termial markdown generator for gfm */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

static int line_wrap = 79;

struct md_state {
    enum {
        MD_PLAIN,
        MD_H,
        MD_CODE_SPAN,
        MD_CODE_BLOCK,
    } state;
    int hlevel;
    int col;
    bool line_start;
    bool seen_nl;
    bool inws;
};

static void putch_par(struct md_state *s, char ch, FILE *out, bool collapsews) {
    if (s->line_start) {
        fputs("    ", out);
        s->col += 4;
        s->line_start = false;
    } else if (s->col >= line_wrap) {
        fputs("\n    ", out);
        s->col = 4;
    }

    if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        if (!s->inws || !collapsews) {
            s->inws = true;
            fputc(' ', out);
            s->col++;
        }

        if (ch == '\n') {
            if (s->seen_nl) {
                s->seen_nl = false;
                fputs("\n\n", out);
                s->line_start = true;
                s->col = 0;
            } else {
                s->seen_nl = true;
            }
        }
    } else {
        s->seen_nl = false;
        s->inws = false;
        fputc(ch, out);
        s->col++;
    }

}

static char capitalize(char c) {
    if (c >= 'a' && c <= 'z') {
        return c + ('A' - 'a');
    }
    return c;
}

static int gen_md(FILE *in, FILE *out) {
    struct md_state pstate = { MD_PLAIN, 0, 0, true, false, true};

    while (true) {
        char ch = fgetc(in);
        if (ch == EOF) {
            break;
        }

        if (pstate.state == MD_PLAIN) {
            md_plain:
            if (ch == '#' && pstate.line_start) {
                pstate.state = MD_H;
                pstate.inws = true;
                pstate.hlevel = 1;
            } else if (ch == '`') {
                pstate.state = MD_CODE_SPAN;
                fputs("\e[1m", out);
            } else {
                putch_par(&pstate, ch, out, true);
            }
        } else if (pstate.state == MD_H) {
            if (ch == '#' && pstate.line_start) {
                pstate.hlevel++;
            } else if (pstate.line_start && ch == ' ') {
            } else if (pstate.line_start) {
                pstate.line_start = false;
                fputs("\e[1m", out);
                for (int i = 0; i < pstate.hlevel - 1; ++i) {
                    fputc(' ', out);
                }
                if (pstate.hlevel == 1) {
                    fputc(capitalize(ch), out);
                } else {
                    fputc(ch, out);
                }
            } else if (ch == '\n') {
                pstate.seen_nl = true;
            } else if (pstate.seen_nl && ch != '\n') {
                fputs("\n\e[22m", out);
                pstate.line_start = true;
                pstate.seen_nl = false;
                pstate.inws = true;
                pstate.state = MD_PLAIN;
                goto md_plain;
            } else {
                if (pstate.hlevel == 1) {
                    fputc(capitalize(ch), out);
                } else {
                    fputc(ch, out);
                }
            }
        } else if (pstate.state == MD_CODE_SPAN) {
            if (ch == '`') {
                pstate.state = MD_PLAIN;
                fputs("\e[22m", out);
            } else {
                putch_par(&pstate, ch, out, false);
            }
        } else if (pstate.state == MD_CODE_BLOCK) {
        }

    }

    fputs("\e[0m\n", out);

    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "w:")) != -1) {
        if (opt == 'w') {
            line_wrap = atoi(optarg);
        }
    }

    FILE *inf = stdin;
    if (argc-optind == 1) {
        inf = fopen(argv[optind], "r");
        if (inf == NULL) {
            perror("fopen(3)");
            return 1;
        }
    }

    gen_md(inf, stdout);

    fclose(inf);

    return 0;
}
