#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include "../cpu.h"


struct pushed_regs {
    int32_t  vecn;
    int32_t  exception;
    int32_t  irq;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t error_code; // -1 when there is no error code.
    uint32_t eip;
} __attribute__((packed));

typedef void(*interrupt_handler_t)(struct pushed_regs *regs);

void send_eoi(int irq);

void clear_interrupt_handlers(void);

void register_interrupt_handler(int vecn, interrupt_handler_t handler);

void general_interrupt_handler(struct pushed_regs regs);

void PIC_remap(int offset1, int offset2);

void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);

#endif
