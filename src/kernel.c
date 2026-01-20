#include <kernel.h>
#include <multiboot2.h>

/*
 * Reference here for the tag system and handling from the grub2
 * https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#kernel_002ec
 * */

void kernel_main(uint32_t magic, uint32_t mbi)
{
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        printk("MAGIC FAILED: %X\n", magic);
        return;
    }
    if (mbi & 7) {
        printk("UNALIGNED MBI: %X\n", mbi);
        return;
    }
    int i = 0;
    (void)i;
    // uint32_t mem_in_mib;

    terminal_initialize();
    terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK, 0));
    serial_init();
    init_cpu_state();
    init_gdt();
    printk("Test writing before reloading segments\n");
    printk("Hello\n");

    while (1) {
        asm volatile("hlt");
    }
}
