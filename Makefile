# Cross-compiler and tools
GCC = ./cross-compiler/bin/i686-elf-gcc
AS = ./cross-compiler/bin/i686-elf-as
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LINKERFLAGS = -ffreestanding -O2 -nostdlib

# Source files
SOURCES_C = src/kernel.c src/gdt.c
SOURCES_S = src/boot.s

# Header files
INCLUDES = src/gdt.h

# Linker script
LINKER = src/linker.ld

# Output kernel image name
KERNEL_IMAGE = myos.bin

# Object files (automatically generated from SOURCES)
OBJECTS_C = $(patsubst src/%.c,obj/%.o,$(SOURCES_C))
OBJECTS_S = $(patsubst src/%.s,obj/%.o,$(SOURCES_S))

# Default target: build the kernel image
all: $(KERNEL_IMAGE)

# Rule for compiling C source files to object files
obj/%.o: src/%.c $(INCLUDES)
	$(GCC) $(CFLAGS) -c $< -o $@

# Rule for compiling assembly source files to object files
obj/%.o: src/%.s
	$(AS) $< -o $@

# Rule for linking object files into the kernel image
$(KERNEL_IMAGE): $(OBJECTS_S) $(OBJECTS_C) $(LINKER)
	$(GCC) -T $(LINKER) -o $(KERNEL_IMAGE) $(LINKERFLAGS) $(OBJECTS_S) $(OBJECTS_C) -lgcc

# Rule for running the kernel in QEMU
run: $(KERNEL_IMAGE)
	qemu-system-i386 -kernel $(KERNEL_IMAGE) -vga vmware -monitor stdio

# Rule for cleaning up (removing object files and the kernel image)
clean:
	rm -f $(OBJECTS_C) $(OBJECTS_S) $(KERNEL_IMAGE)

# Declare phony targets (not actual files)
.PHONY: all run clean
