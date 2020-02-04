#include "../interrupts.h"
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

static void set_gdt_base(struct gdt_entry *ent, uint32_t base) {
    ent->base1 = base & 0xffff;
    ent->base2 = (base >> 16) & 0xff;
    ent->base3 = (base >> 24) & 0xff;
}

static void set_gdt_limit(struct gdt_entry *ent, uint32_t limit) {
    ent->limit1 = limit & 0xffff;
    ent->limit2 = (limit >> 16) & 0xf;
}

static struct gdt_descriptor gdt_desc;
static struct gdt_entry gdt_entries[5];

static void setup_gdt(void) {
    gdt_desc.size = 3;
    gdt_desc.offset = (uintptr_t) gdt_entries;

    // null descriptor
    set_gdt_base(gdt_entries + 0, 0);
    set_gdt_limit(gdt_entries + 0, 0);
    gdt_entries[0].flags = 0xC;
    gdt_entries[0].access_byte = 0;

    // code
    set_gdt_base(gdt_entries + 1, 0);
    set_gdt_limit(gdt_entries + 1, 0xffffffff >> 12);
    gdt_entries[1].flags = 0xC;
    gdt_entries[1].access_byte = 0x9A;

    // data
    set_gdt_base(gdt_entries + 1, 0);
    set_gdt_limit(gdt_entries + 1, 0xffffffff >> 12);
    gdt_entries[1].flags = 0xC;
    gdt_entries[1].access_byte = 0x92;

    //TODO: tss

    /*
    asm volatile ("lgdt %0"
                  :
                  : "a" (&gdt_desc));
    */
}

void init_interrupts(void) {
    cli();
    setup_gdt();
}

/* disable interrupts */
void cli(void) {
    asm("cli");
}

/* enable interrupts */
void sti(void) {
    asm("sti");
}

typedef void(*keypress_cb_t)(int scancode);

void register_keypress(keypress_cb_t callback);
