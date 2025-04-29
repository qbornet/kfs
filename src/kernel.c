#include "kernel.h"
#include "gdt.h"
#include "vga/vga.h"

void kernel_main(void)
{
    init_gdt();
	terminal_initialize();
	terminal_writestring("Hello, World From Kernel!", vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK, 0));

    while (1) { 
        asm volatile ("hlt");
    }
}
