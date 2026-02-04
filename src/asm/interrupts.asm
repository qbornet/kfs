global isr14
extern page_fault_handler

isr14:
    cli
    pusha           ; Save general purposes registers
    push esp        ; Pass stack pointer to C
    call page_fault_handler
    pop esp
    popa
    add esp, 4
    iret
