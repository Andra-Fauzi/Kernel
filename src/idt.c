#include <stdint.h>
#include <stdbool.h>
#include "idt.h"

typedef struct {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t isr_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

extern void terminal_write_string(const char* str);
extern void terminal_putchar(char c);

// Define IDT_SIZE
#define IDT_SIZE 256

// Declare the IDT as a global variable, correctly sized.  Alignment is important.
__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_SIZE];

// Declare the IDTR as a global variable.
static idtr_t idtr;

// Function to set a single IDT entry.
void idt_set_descriptor(uint8_t vector, uint32_t isr, uint16_t sel, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector]; // Use the vector

    descriptor->isr_low = isr & 0xFFFF;
    descriptor->kernel_cs = sel;        // Use the provided selector
    descriptor->attributes = flags;
    descriptor->isr_high = (isr >> 16) & 0xFFFF;
    descriptor->reserved = 0;
}

// Divide-by-zero exception handler.
void divide_by_zero_handler() {
    terminal_write_string("Divide-by-zero handler called!\n");
    // Removed hlt for testing
    __asm__ volatile ("cli; hlt");
}

// General exception handler that handles multiple interrupts
void exception_handler() {
    terminal_write_string("Generic exception handler called.\n");
    __asm__ volatile ("cli; hlt");
    
    // Simulate triggering interrupt 0x6 to invoke the general protection fault handler again
    // __asm__ volatile ("int $0x6");
}

// Function to initialize the IDT.
void idt_init() {
    idtr.limit = (sizeof(idt_entry_t) * IDT_SIZE) - 1;
    idtr.base = (uintptr_t)&idt[0];

    // Set exception handlers (0–31)
    extern void (*isr_stub_table[])();
    for (int i = 0; i < 32; i++) {
        if(i == 0)
        {
            idt_set_descriptor(i, (uint32_t)divide_by_zero_handler, 0x08, 0x8E);
        }
        else 
        {
            idt_set_descriptor(i, (uint32_t)exception_handler,0x08,0x8E);
        }
    }

    // // Optional: Set default handlers for 32–255
    // for (int i = 32; i < IDT_SIZE; i++) {
    //     idt_set_descriptor(i, (uint32_t)exception_handler, 0x08, 0x8E);
    // }

    __asm__ volatile ("lidt %0" : : "m" (idtr));
    __asm__ volatile ("sti");
}
