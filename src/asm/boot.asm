section .multiboot
    align 8

multiboot_header:
    dd 0xE85250D6                                           ; Magic
    dd 0                                                    ; Architecture
    dd multiboot_header_end - multiboot_header              ; Header length
    dd -(0xE85250D6 + (multiboot_header_end - multiboot_header))
    
    ; End tag (must be exactly 8 bytes)
    align 8
    dw 0        ; type = 0
    dw 0        ; flags = 0
    dd 8        ; size = 8
multiboot_header_end:

section .bss
    align 16
global stack_bottom
global stack_top
stack_bottom:
    resb 0x4000
stack_top:

section .text
extern kernel_main
global _start

_start:
    ; Disable interrupts
    cli

    ; Set up stack
    mov esp, stack_top

    ; MBI INFO
    push ebx    ; MBI address
    push eax    ; Magic number

    call kernel_main

.hang:
    hlt
    jmp .hang
