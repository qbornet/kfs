#include "kernel.h"

/*
 * Reference here for the tag system and handling from the grub2
 * https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#kernel_002ec
 * */

void kernel_main(uint32_t mbi)
{
    if (mbi & 7) {
        printk("UNALIGNED MBI: %lX\n", mbi);
        return;
    }
    // uint32_t mem_in_mib;

    terminal_initialize();
    terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK, 0));
    serial_init();
    init_cpu_state();
    init_gdt();
    // mem_in_mib = get_memory_max_value(mbi);
    // init_paging(mem_in_mib);
    //  init_malloc_ptr();

    // int        val = 42;
    // struct gdt gdt_ptr;
    // asm volatile("sgdt %0"
    //              : "=m"(gdt_ptr));
    // print_memory((void *)0x800, 0x40);
    // print_memory((void *)gdt_ptr.address, 0x40);
    // print_memory((void *)&val, 0x02);
    // printk("\nCPL: %X\n", get_current_level_privilege());
    // jump_usermode();
    // uint32_t *ptr = get_virtual_address();
    // if (ptr == NULL) {
    //     printk("VIRTUAL ADDRESS FETCH DONE\n");
    // } else {
    //     printk("virtual addres is: %p\n", ptr);
    // }
    while (1) {
        asm volatile("hlt");
    }
}
