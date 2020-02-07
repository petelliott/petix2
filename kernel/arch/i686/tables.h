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

void do_lgdt(struct gdt_descriptor *);

void setup_gdt(void);

#endif
