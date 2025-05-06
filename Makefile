# Cross-compiler and tools
GCC = ./cross-compiler/bin/i686-elf-gcc
ASM = nasm
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iincludes
LINKERFLAGS = -ffreestanding -O2 -nostdlib

# Source files
SOURCES_C = src/kernel.c src/gdt.c src/idt.c
SOURCES_S = src/boot.asm

# Header files (optional include)
INCLUDES = includes/gdt.h

# Linker script
LINKER = src/linker.ld

# Output kernel image name
KERNEL_IMAGE = myos.bin

# Object files
OBJECTS_C = $(patsubst src/%.c,obj/%.o,$(SOURCES_C))
OBJECTS_S = $(patsubst src/%.asm,obj/%.o,$(SOURCES_S))
OBJECTS = $(OBJECTS_C) $(OBJECTS_S)

# Default target
all: $(KERNEL_IMAGE)

# Create obj/ directory if not exists
obj:
	mkdir -p obj

# Compile C files
obj/%.o: src/%.c | obj
	$(GCC) $(CFLAGS) -c $< -o $@

# Compile NASM assembly files
obj/%.o: src/%.asm | obj
	$(ASM) -f elf32 $< -o $@

# Link everything
$(KERNEL_IMAGE): $(OBJECTS) $(LINKER)
	$(GCC) -T $(LINKER) -o $@ $(LINKERFLAGS) $(OBJECTS) -lgcc

# Run with QEMU
run: $(KERNEL_IMAGE)
	qemu-system-i386 -kernel $(KERNEL_IMAGE) -vga vmware -monitor stdio

# Clean up
clean:
	rm -f $(OBJECTS) $(KERNEL_IMAGE)

.PHONY: all run clean
