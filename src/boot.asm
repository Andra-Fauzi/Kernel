; Multiboot header
%define ALIGN      (1 << 0)
%define MEMINFO    (1 << 1)
%define FLAGS      (ALIGN | MEMINFO)
%define MAGIC      0x1BADB002
%define CHECKSUM   -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; BSS section for the stack
section .bss
align 16
stack_bottom:
    resb 16384
stack_top:


; Code section
section .text
global _start
extern gdt_setup
extern gdt_ptr
extern kernel_main
global load_gdt

extern exception_handler

extern exception_handler

%macro isr_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret
%endmacro

global isr_stub_table
isr_stub_table:
    dd isr_stub_0
    dd isr_stub_1
    dd isr_stub_2
    dd isr_stub_3
    dd isr_stub_4
    dd isr_stub_5
    dd isr_stub_6
    dd isr_stub_7
    dd isr_stub_8
    dd isr_stub_9
    dd isr_stub_10
    dd isr_stub_11
    dd isr_stub_12
    dd isr_stub_13
    dd isr_stub_14
    dd isr_stub_15
    dd isr_stub_16
    dd isr_stub_17
    dd isr_stub_18
    dd isr_stub_19
    dd isr_stub_20
    dd isr_stub_21
    dd isr_stub_22
    dd isr_stub_23
    dd isr_stub_24
    dd isr_stub_25
    dd isr_stub_26
    dd isr_stub_27
    dd isr_stub_28
    dd isr_stub_29
    dd isr_stub_30
    dd isr_stub_31

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31


_start:
    mov esp, stack_top
    cli

    ; Load GDT
    call gdt_setup
    mov eax, gdt_ptr
    push dword eax
    call load_gdt

; Function to load GDT
load_gdt:
    cli
    mov eax, [esp + 4]     ; Get address of GDT pointer
    lgdt [eax]             ; Load GDT

    ; Set CR0 to enter protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Jump to protected mode entry point
    jmp 0x08:protected_mode_entry

; Protected mode entry
protected_mode_entry:
    ; Reinitialize segment registers
    mov ax, 0x10             ; Load the data segment selector (entry 2 in GDT)
    mov ds, ax               ; Set DS (data segment)
    mov es, ax               ; Set ES (extra segment)
    mov fs, ax               ; Set FS (for thread-local storage)
    mov gs, ax               ; Set GS (for kernel-specific data)
    mov ss, ax               ; Set SS (stack segment)

    ; Now we're in protected mode, we can call kernel_main
    call kernel_main
