section .multiboot
    align 8
 
;Multiboot2 header:
;magic: 0xE85250D6
;architecture: 0 (i386)
;header_length: 24 (bytes total header including the end tag)
;checksum: such that the sum of the four values equals 0 modulo 2^32.
multiboot_header:
    dd 0xE85250D6         ; Multiboot2 magic number
    dd 0                  ; Architecture (0 = i386)
    dd 24                 ; Total header length (in bytes)
    dd 0x17ADAF12         ; Checksum: (0xE85250D6 + 0 + 24 + 0x17ADAF12 = 0 mod 2^32)
    dw 6                  ; type = 6, modules alignment
    dw 0                  ; flags optional = 0.
    dd 8                  ; Tag size is 8 bytes

section .bss              ; start scetion .bss create stack
align 16                  ; align 16 byte to follow System V ABI
stack_bottom:             
resb 0x4000               ; 16 KiB
stack_top:

section .text
    global _start
    extern kernel_main

_start:
    call kernel_main
hang:
    jmp hang
