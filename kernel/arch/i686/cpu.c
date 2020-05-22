#include "../cpu.h"
#include <stdint.h>
#include "tables.h"
#include "interrupts.h"
#include <stddef.h>
#include "io.h"

static void keypress_int_handler(int vecn, int excep, int irq);

void init_cpu(void) {
    cli();
    clear_interrupt_handlers();
    setup_gdt();
    setup_idt();
    register_interrupt_handler(33, keypress_int_handler);
}

/* disable interrupts */
void cli(void) {
    asm("cli");
}

/* enable interrupts */
void sti(void) {
    asm("sti");
}

void halt(void) {
    asm("hlt");
}

static keypress_cb_t keyboard_callback = NULL;

static void keypress_int_handler(int vecn, int excep, int irq) {
    cli();
    uint8_t sc = inb(0x60);
    if (keyboard_callback != NULL) {
        keyboard_callback(sc);
    }
    send_eoi(irq);
    sti();
}

void register_keypress(keypress_cb_t callback) {
    keyboard_callback = callback;
}
