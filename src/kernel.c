#include "kernel.h"

/*
 * Reference here for the tag system and handling from the grub2
 * https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#kernel_002ec
 * */

/*
size = *(uint32_t *)mbi;
printk("Announced mbi size: %X\n", size);
for (tag = (struct multiboot_tag *)(mbi + 8);
     tag->type != MULTIBOOT_TAG_TYPE_END;
     tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag
                                    + ((tag->size + 7) & ~7))) {
    printk("Tag: %X, Size: %X\n", tag->type, tag->size);
    switch (tag->type) {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
            {
                struct multiboot_tag_string *tag_string
                    = (struct multiboot_tag_string *)tag;
                printk("Command line = %s\n", tag_string->string);
                break;
            }
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            printk("Boot loader name = %s\n",
                   ((struct multiboot_tag_string *)tag)->string);
            break;
        case MULTIBOOT_TAG_TYPE_MODULE:
            printk("Boot at 0x%X-0x%X. Command line %s\n",
                   ((struct multiboot_tag_module *)tag)->mod_start,
                   ((struct multiboot_tag_module *)tag)->mod_end,
                   ((struct multiboot_tag_module *)tag)->cmdline);
            break;
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            {
                uint32_t total_mem
                    = ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower
                    + ((struct multiboot_tag_basic_meminfo *)tag)
                          ->mem_upper;
                printk(
                    "Total_mem: %lu KiB lower_mem: %u, upper_mem: %u\n",
                    total_mem,
                    ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
                    ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
                break;
            }
        case MULTIBOOT_TAG_TYPE_APM:
            {
                // struct multiboot_tag_apm *apm_table
                //     = (struct multiboot_tag_apm *)tag;
                break;
            }
        default: break;
    }
}
*/

void kernel_main(uint32_t magic, uint32_t mbi)
{
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        printk("MULTIBOOT2_HEADER_FAILED: %lX\n", magic);
        return;
    }
    if (mbi & 7) {
        printk("UNALIGNED MBI: %lX\n", mbi);
        return;
    }
    // struct multiboot_tag *tag;
    // unsigned size;

    terminal_initialize();
    terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK, 0));
    serial_init();
    init_cpu_state();
    init_gdt();

    int        val = 42;
    struct gdt gdt_ptr;
    asm volatile("sgdt %0"
                 : "=m"(gdt_ptr));
    serial_write("toto\n");
    print_memory((void *)0x800, 0x40);
    print_memory((void *)gdt_ptr.address, 0x40);
    print_memory((void *)&val, 0x02);
    printk("\nCPL: %X\n", get_current_level_privilege());
    // jump_usermode();
    while (1) {
        asm volatile("hlt");
    }
}
