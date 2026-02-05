OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    . = 1M;

    .text : ALIGN(4K) {
        KEEP(*(.multiboot))
        *(.text)
    }

    .bss : ALIGN(4K) {
        *(.bss)
    }

    .data : ALIGN(4K) {
        *(.data)
    }
}
