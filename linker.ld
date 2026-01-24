OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

/* Constants */
VIRTUAL_BASE = 0xC0000000;
PHYSICAL_BASE = 0x00100000;
KERNEL_OFFSET = 0xBFF00000; /* VIRTUAL_BASE - PHYSICAL_BASE */

SECTIONS
{
    /* Start at 1MiB */
    . = PHYSICAL_BASE;
    .init.bss : ALIGN(4K) 
    {
        /* This contains the boot_stack before jumping in higher half kernel */
        *(.init.bss)
    }
    .boot : ALIGN(4K)
    {
        *(.multiboot)
        *(.boot)
    }
    _boot_end_phys = .;

    /* --- Higher Half Sections --- */
    
    /* Set the Virtual Counter to 3GB + offset from boot code */
    . = VIRTUAL_BASE + (_boot_end_phys - PHYSICAL_BASE);

    /* * STRATEGY: 
     * 1. Align the virtual counter (.)
     * 2. Save the current virtual address to a symbol (_vma_text, etc.)
     * 3. Define section with AT(_vma_symbol - KERNEL_OFFSET)
     * This forces the LMA to be exactly (Virtual - 0xBFF00000).
     */

    . = ALIGN(4K);
    _vma_text = .;
    .text : AT(_vma_text - KERNEL_OFFSET)
    {
        *(.text)
        *(.text.*)
    }

    . = ALIGN(4K);
    _vma_rodata = .;
    .rodata : AT(_vma_rodata - KERNEL_OFFSET)
    {
        *(.rodata)
        *(.rodata.*)
    }

    . = ALIGN(4K);
    _vma_data = .;
    .data : AT(_vma_data - KERNEL_OFFSET)
    {
        *(.data)
        *(.data.*)
        /* Catch global offset tables to prevent overlaps */
        *(.got*) 
        *(.plt*)
    }

    /*
    . = ALIGN(4K);
    _vma_eh_frame = .;
    .eh_frame : AT(_vma_eh_frame - KERNEL_OFFSET)
    {
        *(.eh_frame)
        *(.eh_frame.*)
    }
    */

    . = ALIGN(4K);
    _vma_bss = .;
    .bss : AT(_vma_bss - KERNEL_OFFSET)
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
    g_kernel_end = .;

    /* Mark the end of the kernel */

    /DISCARD/ : { *(.eh_frame) *(.comment) *(.note*) }
}
