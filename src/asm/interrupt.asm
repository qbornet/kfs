[bits 32]
section .text

extern interrupt_handler
global isr_common_stub

; Macro for interrupts that dont push an error code (push dummy 0)
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0  ; Dummy error code
    push %1 ; Interrupt number
    jmp isr_common_stub
%endmacro

; Macro for interrupts that do push an error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    ; Error code is already push by CPU
    push %1 ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Define ISRs 0 to 21
ISR_NOERRCODE 0        ; Divide by Zero
ISR_NOERRCODE 1        ; Debug
ISR_NOERRCODE 2        ; Non-maskable Interrupt
ISR_NOERRCODE 3        ; Breakpoint
ISR_NOERRCODE 4        ; Overflow
ISR_NOERRCODE 5        ; Bound Range Exceeded
ISR_NOERRCODE 6        ; Invalid Opcode
ISR_NOERRCODE 7        ; Device Not Available
ISR_ERRCODE   8        ; Double Fault
ISR_NOERRCODE 9        ; Coprocessor Segment Overrun
ISR_ERRCODE   10       ; Invalid TSS
ISR_ERRCODE   11       ; Segment Not Present
ISR_ERRCODE   12       ; Stack-Segment Fault
ISR_ERRCODE   13       ; General Protection Fault 
ISR_ERRCODE   14       ; Page Fault
ISR_NOERRCODE 15       ; Reserved
ISR_NOERRCODE 16       ; x87 Floating-Point Exception
ISR_ERRCODE   17       ; Alignment Check
ISR_NOERRCODE 18       ; Machine Check
ISR_NOERRCODE 19       ; SIMD Floating-Point Exception
ISR_NOERRCODE 20       ; Virutalization Exception
ISR_ERRCODE   21       ; Control Protection Exception

isr_common_stub:
    pushad      ; Pushes GPRs

    push ds     ; Save data segment
    push es
    push fs
    push gs

    push esp    ; Pass pointer to IsrContext struct
    cld         ; SysV ABI requiresa DF to be clear (Direction Flags)
    call interrupt_handler

    add esp, 4  ; Clean up the pushed ESP pointer

    pop gs      ; Restore segments
    pop fs
    pop es
    pop ds

    popad       ; Restore GPRs
    add esp, 8  ; Clean up pushed interrupt number and error code
    iret        ; Return to interrupted code

