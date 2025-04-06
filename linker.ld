OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    /* Place the kernel at 1MB */
    . = 0x00100000;

    .multiboot : {
         KEEP(*(.multiboot))
    }

    .text : {
         *(.text*)
    }

    .rodata : {
         *(.rodata*)
    }

    .data : {
         *(.data*)
    }

    .bss : {
         *(COMMON)
         *(.bss*)
    }
}
