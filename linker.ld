OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
VIRTUAL_BASE = 0xC0000000;
PHYSICAL_BASE = 0x00100000;

SECTIONS
{
    /* Place the kernel at 1MB */
    . = PHYSICAL_BASE;

    .boot ALIGN(4K) :
    {
        *(.multiboot)
        *(.boot)
    }
    .debug_info ALIGN(4K) : 
    {
        *(.debug_info)
    }
    .debug_line ALIGN(4K) :
    {
        *(.debug_line)
    }

    . = ALIGN(4K);
    _boot_end = .;

    . = VIRTUAL_BASE + (_boot_end - PHYSICAL_BASE);

    .text ALIGN(4K) : AT(_boot_end)
    {
        _text_start = .;
        *(.text)
        _text_end = .;
    }

    . = ALIGN(4K);
    _text_end_phys = _boot_end + SIZEOF(.text);

    .rodata ALIGN(4K) : AT(_text_end_phys)
    {
        *(.rodata)
    }

    . = ALIGN(4K);
    _rodata_end_phys = _text_end_phys + SIZEOF(.rodata);

    .data ALIGN(4K) : AT(_rodata_end_phys)
    {
         *(.data)
    }

    . = ALIGN(4K);
    _data_end_phys = _rodata_end_phys + SIZEOF(.data);

    .bss ALIGN(4K) : AT(_data_end_phys)
    {
         *(COMMON)
         *(.bss)
         g_kernel_end = .;
    }
}
