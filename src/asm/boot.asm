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

section .boot
align 4096
global g_page_directory
g_page_directory:
    times 1024 dd 0

align 4096
global g_page_table
g_page_table:
    times 1024 dd 0

; Boot stack in low memory

align 16
boot_stack_bottom:
    resb 0x4000     ; 16KB
boot_stack_top:

section .boot
global _start
global setup_identity_paging
global enable_paging
extern kernel_early_main

_start:
    ; Disable interrupts
    cli
    
    ; Set up boot stack (in low memory)
    mov esp, boot_stack_top
    
    ; Save multiboot info
    push ebx    ; MBI address
    push eax    ; Magic number

    ; Set up paging in assembly
    call setup_identity_paging
    call enable_paging
    
    ; Call early kernel
    call kernel_early_main
    
    ; Should never return
    cli
.hang:
    hlt
    jmp .hang

; Function to set up identity mapping and higher half mapping
setup_identity_paging:
    push ebp
    mov ebp, esp
    push edi
    push esi
    push ebx
    push ecx
    push edx

    ; Clear page directory
    mov edi, g_page_directory
    mov ecx, 1024
    xor eax, eax
    rep stosd

    ; Fill page table - map first 4MB
    mov edi, g_page_table
    xor ebx, ebx            ; Physical address counter
    mov ecx, 1024
.fill_page_table:
    mov eax, ebx
    or eax, 0x003           ; Present, R/W
    stosd
    add ebx, 0x1000         ; Next 4KB page
    loop .fill_page_table

    ; Get physical address of page table
    ; We need to ensure this is a physical address
    mov eax, g_page_table
    
    ; Check if this is already a physical address (< 0x40000000)
    cmp eax, 0x40000000
    jb .is_physical
    
    ; If it's a virtual address, convert to physical
    sub eax, 0xC0000000
    
.is_physical:
    or eax, 0x003           ; Present, R/W
    
    ; Set up identity mapping at PD[0] (maps 0x00000000-0x003FFFFF)
    mov [g_page_directory], eax
    
    ; Set up higher half mapping at PD[768] (maps 0xC0000000-0xC03FFFFF)
    mov [g_page_directory + 768 * 4], eax
    
    pop edx
    pop ecx
    pop ebx
    pop esi
    pop edi
    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp
    push eax

    ; Ensure we're using physical address for page directory
    mov eax, g_page_directory
    
    ; Check if this is already a physical address
    cmp eax, 0x40000000
    jb .load_cr3
    
    ; Convert to physical if needed
    sub eax, 0xC0000000
    
.load_cr3:
    mov cr3, eax
    
    ; Enable paging by setting bit 31 in CR0
    mov eax, cr0
    or eax, 0x80000000      ; Use hex instead of bit shift
    mov cr0, eax
    
    ; CPU is now using paging!
    
    pop eax
    pop ebp
    ret

; Jump to higher half
global jump_to_higher_half
extern kernel_main
extern g_saved_mbi_addr

jump_to_higher_half:
    ; We need to jump to an absolute address in higher half
    ; Calculate the address properly
    mov eax, higher_half ; Adjust based on where .text is loaded
    jmp eax

section .bss
align 16
global stack_bottom
global stack_top
stack_bottom:
    resb 0x4000
stack_top:

section .text
higher_half:
    ; Now executing at 0xC0000000+
    ; Fix stack pointer to higher half
    mov eax, stack_top      ; Get higher-half stack address
    mov esp, eax            ; Use the proper higher-half stack
    
    ; Remove identity mapping
    call remove_identity_mapping
    
    ; Call kernel main with saved MBI
    push dword [g_saved_mbi_addr]
    call kernel_main
    
    ; Halt if kernel returns
    cli
.hang:
    hlt
    jmp .hang

; Remove identity mapping function
global remove_identity_mapping
remove_identity_mapping:
    ; Clear the identity mapping at PD[0]
    ; g_page_directory is now accessed at its virtual address
    mov dword [g_page_directory + 0xC0000000], 0
    
    ; Flush TLB by reloading CR3
    mov eax, cr3
    mov cr3, eax
    
    ret
