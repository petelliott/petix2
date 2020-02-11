#include "interrupts.h"
#include "../../kdebug.h"
#include "io.h"
#include <stddef.h>


#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */

void send_eoi(int irq) {
    if(irq >= 8) {
		outb(PIC2_COMMAND,PIC_EOI);
    }

	outb(PIC1_COMMAND,PIC_EOI);
}

keypress_cb_t keyboard_callback = NULL;

void general_irq_handler(struct pushed_regs regs) {
    if (regs.irq != 0) {
        kprintf("got irq: %i\n", regs.irq);
    }

    if (regs.irq == 1) {
        // keyboard interrupt

        //TODO reentrant interrupts
        uint8_t sc = inb(0x60);
        if (keyboard_callback != NULL) {
            keyboard_callback(sc);
        }
        send_eoi(regs.irq);
    } else {
        send_eoi(regs.irq);
    }
}

void general_exception_handler(struct pushed_regs regs) {
    kprintf("got exception: %i\n", regs.exception);
    panic("unhandlable exception");
}

// from osdev wiki
/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

void PIC_remap(int offset1, int offset2) {
	unsigned char a1, a2;

	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();

	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

uintptr_t isrs[48] = {
    (uintptr_t) excep0,
    (uintptr_t) excep1,
    (uintptr_t) excep2,
    (uintptr_t) excep3,
    (uintptr_t) excep4,
    (uintptr_t) excep5,
    (uintptr_t) excep6,
    (uintptr_t) excep7,
    (uintptr_t) excep8,
    (uintptr_t) excep9,
    (uintptr_t) excep10,
    (uintptr_t) excep11,
    (uintptr_t) excep12,
    (uintptr_t) excep13,
    (uintptr_t) excep14,
    (uintptr_t) excep15,
    (uintptr_t) excep16,
    (uintptr_t) excep17,
    (uintptr_t) excep18,
    (uintptr_t) excep19,
    (uintptr_t) excep20,
    (uintptr_t) excep21,
    (uintptr_t) excep22,
    (uintptr_t) excep23,
    (uintptr_t) excep24,
    (uintptr_t) excep25,
    (uintptr_t) excep26,
    (uintptr_t) excep27,
    (uintptr_t) excep28,
    (uintptr_t) excep29,
    (uintptr_t) excep30,
    (uintptr_t) excep31,
    (uintptr_t) irq0,
    (uintptr_t) irq1,
    (uintptr_t) irq2,
    (uintptr_t) irq3,
    (uintptr_t) irq4,
    (uintptr_t) irq5,
    (uintptr_t) irq6,
    (uintptr_t) irq7,
    (uintptr_t) irq8,
    (uintptr_t) irq9,
    (uintptr_t) irq10,
    (uintptr_t) irq11,
    (uintptr_t) irq12,
    (uintptr_t) irq13,
    (uintptr_t) irq14,
    (uintptr_t) irq15
};
