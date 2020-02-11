#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>


struct pushed_regs {
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

void general_irq_handler(struct pushed_regs regs);
void general_exception_handler(struct pushed_regs regs);
void PIC_remap(int offset1, int offset2);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void excep0(void);
extern void excep1(void);
extern void excep2(void);
extern void excep3(void);
extern void excep4(void);
extern void excep5(void);
extern void excep6(void);
extern void excep7(void);
extern void excep8(void);
extern void excep9(void);
extern void excep10(void);
extern void excep11(void);
extern void excep12(void);
extern void excep13(void);
extern void excep14(void);
extern void excep15(void);
extern void excep16(void);
extern void excep17(void);
extern void excep18(void);
extern void excep19(void);
extern void excep20(void);
extern void excep21(void);
extern void excep22(void);
extern void excep23(void);
extern void excep24(void);
extern void excep25(void);
extern void excep26(void);
extern void excep27(void);
extern void excep28(void);
extern void excep29(void);
extern void excep30(void);
extern void excep31(void);

extern uintptr_t isrs[48];

#endif
