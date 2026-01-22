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
    printk("Kernel Loaded. RAM Detecded: %u MiB / %u B\n",
           boot_info->max_memory / 1024 / 1024,
           boot_info->max_memory);

    uint8_t  to_save = 0;
    uint32_t size = 0;
    uint32_t highest_size = 0;
    printk("mmap_max_size %d\n", boot_info->mmap_max_size);
    for (uint8_t i = 0; i < boot_info->mmap_max_size; i++) {
        mmap_info_t mmap_info = boot_info->mmap_info[i];
        size = mmap_info.high_len + mmap_info.low_len;
        if (size > highest_size) {
            highest_size = size;
            to_save = i;
        }
        printk("mmap_info: %p\n", &mmap_info);
        printk("mmap_address: 0x%x%x size: 0x%x%x ",
               mmap_info.base_high,
               mmap_info.base_low,
               mmap_info.high_len,
               mmap_info.low_len);
        if (mmap_info.type == MULTIBOOT_MEMORY_AVAILABLE) {
            printk("mmap is available\n");
        }
    }
    uint32_t base_addrs = boot_info->mmap_info[to_save].base_high
                        + boot_info->mmap_info[to_save].base_low + 0x400000;
    size -= 0x7c000; // Already map kernel mem.
    printk("base_addrs: 0x%x, highest_size: 0x%x\n", base_addrs, highest_size);
    while (1) {
        asm volatile("hlt");
    }
    init_paging(base_addrs, size);
}
