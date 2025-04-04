org 0x7c00
bits 16

section .multiboot
align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 3)

xor ax, ax
mov ds, ax
cld

mov si, msg
call ch_loop

hang:
    jmp hang

msg db "NILBOGOS", 0

ch_loop: lodsb
    or al, al
    jz done
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x04
    int 0x10
    jmp ch_loop
done:
    ret


times 510-($-$$) db 0
dw 0xAA55
