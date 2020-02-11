#include "../cpu.h"
#include <stdint.h>
#include "tables.h"

void init_cpu(void) {
    cli();
    setup_gdt();
    setup_idt();
}

/* disable interrupts */
void cli(void) {
    asm("cli");
}

/* enable interrupts */
void sti(void) {
    asm("sti");
}

typedef void(*keypress_cb_t)(int scancode);

void register_keypress(keypress_cb_t callback);
