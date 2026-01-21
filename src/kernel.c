#include <boot/parser_mutliboot.h>
#include <kernel.h>
#include <multiboot2.h>

/*
 * Reference here for the tag system and handling from the grub2
 * https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#kernel_002ec
 * */

// Note we dont check bootloader magic and parse mbi here in kernel_main this is
// done in `kernel_early_main()` and pass a struct.
void kernel_main(boot_info_t *boot_info)
{
    terminal_initialize();
    terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK, 0));
    serial_init();
    init_cpu_state();
    init_gdt();
    printk("Kernel Loaded. RAM Detecded: %u MB / %u B",
           boot_info->max_memory / 1024 / 1024,
           boot_info->max_memory);

    // init_paging(boot_info->max_memory);

    while (1) {
        asm volatile("hlt");
    }
}
