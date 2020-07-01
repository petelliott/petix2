#include "../cpu.h"
#include <stdint.h>
#include "tables.h"
#include "interrupts.h"
#include <stddef.h>
#include "io.h"
#include "syscall.h"

static void keypress_int_handler(struct pushed_regs *regs);

void init_cpu(void) {
    cli();
    clear_interrupt_handlers();
    setup_gdt();
    setup_idt();
    register_interrupt_handler(33, keypress_int_handler);
    register_interrupt_handler(0x80, syscall_interrupt_handler);

    asm ("    mov %%cr0, %%eax\n"
         "    or  $0x20, %%eax\n"
         "    mov %%eax, %%cr0\n"
         "    fninit\n"
         ::: "eax");
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

static void keypress_int_handler(struct pushed_regs *regs) {
    cli();
    uint8_t sc = inb(0x60);
    if (keyboard_callback != NULL) {
        keyboard_callback(sc);
    }
    send_eoi(regs->irq);
    sti();
}

void register_keypress(keypress_cb_t callback) {
    keyboard_callback = callback;
}
