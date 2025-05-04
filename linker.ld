OUTPUT_FORMAT(elf32-i386)

ENTRY(_start)
SECTIONS
{
    /* Place the kernel at 1MB */
    . = 0x00100000;

    .multiboot ALIGN(0x1000) :
    {
        KEEP(*(.multiboot))
    }

    . += 0xC0000000;
    
    .text ALIGN(0x1000) : AT(ADDR(.text) - OxC0000000)
    {
         *(.text)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - OxC0000000)
    {
         *(.rodata)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - OxC0000000)
    {
         *(.data)
    }

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - OxC0000000)
    {
         *(COMMON)
         *(.bss)
    }
    
    _kernel_end = .;
}
