#include "kernel.h"
#include "gdt.h"
#include "vga/vga.h"
#include "lib/io.h"

void    kernel_main(void)
{
    init_gdt();
	terminal_initialize();
    terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK, 0));

    struct gdt  gdt_ptr;
    int         val = 42;
    asm volatile (
            "sgdt %0"
            : "=m" (gdt_ptr)
        );
    print_memory((void *)0x800, 0x40);
    print_memory((void *)gdt_ptr.address, 0x40);
    print_memory((void *)&val, 0x02);
    while (1) { 
        asm volatile ("hlt");
    }
}
