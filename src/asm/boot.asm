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

; Boot stack in low memory before switching to high-half kernel.
[section .init.bss nobits align=16] ; nobits = noprogsbits wont store in disk images. will be allocated and initiliaze at load. Needed for bss
extern boot_stack_top
boot_stack_bottom:
    resb 0x4000     ; 16KB
boot_stack_top:

section .boot
align 4096
global g_page_directory
g_page_directory:
    times 1024 dd 0

align 4096
global g_page_table
g_page_table:
    times 1024 dd 0

align 4096
global g_page_table_kernel
g_page_table_kernel:
    times 1024 dd 0


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

    ; Fill IDENTITY page table - map first 4MB (0x0 -> 0x0)
    mov edi, g_page_table
    xor ebx, ebx            ; Physical address counter
    mov ecx, 1024
    .fill_page_table:
    mov eax, ebx
    or eax, 0x003           ; Present, R/W
    stosd
    add ebx, 0x1000         ; Next 4KB page
    loop .fill_page_table


    ; Fill KERNEL page table - map physical 1MB-5MB to virtual 0xC0000000
    mov edi, g_page_table_kernel
    mov ebx, 0x100000       ; Start at 1MB physical
    mov ecx, 1024
    .fill_kernel_table:
    mov eax, ebx
    or eax, 0x003           ; Present, R/W
    stosd
    add ebx, 0x1000         ; Next 4KB page
    loop .fill_kernel_table

    ; Install IDENTITY page table at PD[0]
    mov eax, g_page_table
    or eax, 0x003           ; Present, R/W
    mov [g_page_directory], eax

    ; Install KERNEL page table at PD[768]
    mov eax, g_page_table_kernel    ; Use g_page_table_kernel, not g_page_table!
    or eax, 0x003                   ; Present, R/W
    mov [g_page_directory + 768 * 4], eax

    ; Map 0xB8000 (VGA MEMORY) to last page 0xC03FF0000
    mov eax, 0xB8000         ; Physical address of buffer 0xB8000
    or eax, 0x003            ; Present, R/W

    mov [g_page_table_kernel + 1023 * 4], eax

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
    jmp higher_half

section .bss
align 16
global stack_bottom
global stack_top
stack_bottom:
    resb 0x4000
stack_top:


section .text

global higher_half
global remove_identity_mapping

remove_identity_mapping:
    ; g_page_directory address is physical,
    ; We need to get the offset of the g_page_directory because we use virtual address now.
    mov eax, g_page_directory      ; Load physical address
    sub eax, 0x100000              ; Remove physical base
    add eax, 0xC0000000            ; Add virtual base
    
    ; Now EAX points to the Page Directory in virtual memory
    mov dword [eax], 0             

    ; Flush TLB
    mov eax, cr3
    mov cr3, eax
    
    ret

higher_half:
    ; Fix stack pointer to higher half
    mov eax, stack_top

    ; (Optional: if stack symbols are in .bss, they are already virtual in the new linker script
    ;  so simpler: mov esp, stack_top is fine if stack_top is in a higher-half section)

    mov esp, eax 
    call remove_identity_mapping

    extern g_boot_info_ptr
    push dword [g_boot_info_ptr]
    call kernel_main

    cli
.hang:
    hlt
    jmp .hang
