#include "kernel.h"
#include "gdt.h"
#include "vga/vga.h"
#include "lib/io.h"

void kernel_main(void)
{
    init_gdt();
	terminal_initialize();

    struct gdt  gdt_ptr;
    //void        *stack;
    int         val = 42;
    asm volatile (
            "sgdt %0"
            : "=m" (gdt_ptr)
        );
    (void)val;
    printk("[%X]", gdt_ptr.size);
    print_memory((void *)gdt_ptr.address, 0x40);
    printk("[%X]", gdt_ptr.size);

    while (1) { 
        asm volatile ("hlt");
    }
}
