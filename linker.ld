OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    /* Place the kernel at 1MB */
    . = 0x00100000;

    .multiboot BLOCK(0x1000) : ALIGN(0x1000)
    {
        KEEP(*(.multiboot))
    }

    .text BLOCK(0x1000) : ALIGN(0x1000)
    {
         *(.text)
    }

    .rodata BLOCK(0x1000) : ALIGN(0x1000)
    {
         *(.rodata)
    }

    .data BLOCK(0x1000) : ALIGN(0x1000)
    {
         *(.data)
    }

    .bss BLOCK(0x1000) : ALIGN(0x1000)
    {
         *(COMMON)
         *(.bss)
         g_mem_ptr = .;
    }
}
