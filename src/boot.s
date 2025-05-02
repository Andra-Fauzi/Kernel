.set ALIGN,      1 << 0
.set MEMINFO,    1 << 1
.set FLAGS,      ALIGN | MEMINFO
.set MAGIC,      0x1BADB002
.set CHECKSUM,   -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function
.extern gdt_setup
.extern gdt_ptr
.extern kernel_main
.global load_gdt

_start:
    mov $stack_top, %esp
    cli

    call gdt_setup
	.extern gdt_ptr
    push $gdt_ptr
    call load_gdt

    call kernel_main

.halt:
    hlt
    jmp .halt

.size _start, . - _start

load_gdt:
    cli
    mov 4(%esp), %eax
    lgdt (%eax)
    ret
