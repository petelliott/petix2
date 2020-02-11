#include "../cpu.h"
#include <stdint.h>
#include "tables.h"
#include "interrupts.h"

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

void register_keypress(keypress_cb_t callback) {
    keyboard_callback = callback;
}
