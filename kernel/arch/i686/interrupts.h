#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include "../cpu.h"


struct pushed_regs {
    int32_t  vecn;
    int16_t  exception;
    int16_t  irq;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} __attribute__((packed));

extern keypress_cb_t keyboard_callback;

typedef void(*interrupt_handler_t)(int exception, int irq);

// it is the callee's responsibility to reenable interrupts
void register_interrupt_handler(int vecn, interrupt_handler_t handler);

void general_interrupt_handler(struct pushed_regs regs);

void PIC_remap(int offset1, int offset2);

#endif
