#include <stdint.h>
#include <stddef.h>

#include "gdt.h"

// GDT Entry structure (8 bytes)
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) GDTEntry;

// Task State Segment (TSS)
typedef struct {
    uint32_t prev_task_link;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint16_t es, cs, ss, ds, fs, gs;
    uint16_t ldt_selector;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) TSS;

#define GDT_ENTRIES 5

GDTEntry gdt_table[GDT_ENTRIES];
GDTDescriptorPointer gdt_ptr;
TSS tss;

extern void terminal_write_string(const char* str);

// Helper to set a GDT entry
void set_gdt_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt_table[index].base_low    = base & 0xFFFF;
    gdt_table[index].base_middle = (base >> 16) & 0xFF;
    gdt_table[index].base_high   = (base >> 24) & 0xFF;

    gdt_table[index].limit_low   = limit & 0xFFFF;
    gdt_table[index].granularity = ((limit >> 16) & 0x0F) | (flags << 4);
    gdt_table[index].access      = access;
}

void gdt_setup() {
    // Null segment
    set_gdt_entry(0, 0, 0, 0, 0);

    // Code segment: base=0, limit=4GB, access=0x9A (exec/read), flags=0xC (4K granularity, 32-bit)
    set_gdt_entry(1, 0, 0xFFFFF, 0x9A, 0xC);

    // Data segment: base=0, limit=4GB, access=0x92 (read/write), flags=0xC
    set_gdt_entry(2, 0, 0xFFFFF, 0x92, 0xC);

    // User Code (optional): base=0, limit=4GB, DPL=3, access=0xFA
    set_gdt_entry(3, 0, 0xFFFFF, 0xFA, 0xC);

    // TSS entry
    set_gdt_entry(4, (uint32_t)&tss, sizeof(TSS) - 1, 0x89, 0x0);

    gdt_ptr.limit = sizeof(gdt_table) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_table;
}
