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


#endif
