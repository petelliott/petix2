#include "tables.h"
#include "../../kdebug.h"
#include "interrupts.h"
#include <stddef.h>

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

void setup_gdt(void) {
    kassert(sizeof(struct gdt_descriptor) == 6);
    kassert(sizeof(struct gdt_entry) == 8);

    gdt_desc.size = 3*sizeof(struct gdt_entry) - 1;
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
    gdt_entries[2].flags = 0xC;
    gdt_entries[2].access_byte = 0x92;

    //TODO: tss

    //kprintf("%p\n", &gdt_desc);

    asm volatile ("lgdt (%0)"
                  :
                  : "r" (&gdt_desc));
}

static struct gdt_descriptor idt_desc;
static struct idt_entry idt_entries[256] = {0};

void setup_idt(void) {
    kassert(sizeof(struct idt_entry) == 8);
    idt_desc.size = 256*sizeof(struct idt_entry) - 1;
    idt_desc.offset = (uintptr_t) idt_entries;

    //PIC_remap(0,8);
    for (size_t i = 0; i < 48; ++i) {
        idt_entries[i].offset1   = isrs[i] & 0xffff;
        idt_entries[i].selector  = (1 << 3);
        idt_entries[i].zero      = 0;
        idt_entries[i].type_attr = 0x8e;
        idt_entries[i].offset2   = (isrs[i] >> 16) & 0xffff;
    }

    asm volatile ("lidt (%0)"
                  :
                  : "r" (&idt_desc));
}
