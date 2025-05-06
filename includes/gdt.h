#pragma once
#include <stdint.h>

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) GDTDescriptorPointer;

extern void gdt_setup(void);
extern GDTDescriptorPointer gdt_ptr;
