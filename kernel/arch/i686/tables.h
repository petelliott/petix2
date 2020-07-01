#ifndef TABLES_H
#define TABLES_H
#include <stdint.h>

struct gdt_descriptor {
    uint16_t size;
    uint32_t offset;
} __attribute__((__packed__));

struct gdt_entry {
    uint16_t limit1;
    uint16_t base1;
    uint8_t  base2;
    uint8_t  access_byte;
    uint8_t  limit2 : 4;
    uint8_t  flags  : 4;
    uint8_t  base3;
};

void setup_gdt(void);

struct idt_entry {
	uint16_t offset1;
	uint16_t selector;
    uint8_t zero;
	uint8_t type_attr;
	uint16_t offset2;
};

void setup_idt(void);

struct tss {
    uint32_t link;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldtr;
    uint16_t reserved;
    uint16_t iopb;
};

extern struct tss tss;


#endif
