[BITS 32]
global isr_keyboard_stub
extern keyboard_isr          ; C handler

isr_keyboard_stub:
    pusha                    ; save all GP regs
    call  keyboard_isr
    popa
    ; send EOI (IRQ number 1)
    mov  al, 0x20
    out  0x20, al
    iretd                    ; return from interrupt
