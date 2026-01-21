#include "multiboot2.h"
#include <lib/io.h>
#include <multiboot2_info.h>

/*
 * Reference here for the tag system and handling from the grub2
 * https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#kernel_002ec
 *
 * This return the max value of memory available passed via multiboot2,
 * information.
 *
 * @mbi: Multiboot Information pointer (data structure of multiboot2)
 * */
uint32_t get_memory_max_value(uint32_t mbi)
{
    struct multiboot_tag *tag;
    uint32_t              total_mem = 0;
    printk("Announced mbi size 0x%x\n", *(unsigned *)mbi);
    for (tag = (struct multiboot_tag *)(mbi + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag
                                        + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MMAP:
                {
                    multiboot_memory_map_t *mmap;
                    for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
                         (multiboot_uint8_t *)mmap
                         < (multiboot_uint8_t *)tag + tag->size;
                         mmap = (multiboot_memory_map_t
                                     *)((unsigned long)mmap
                                        + ((struct multiboot_tag_mmap *)tag)
                                              ->entry_size)) {
                        // Available RAM
                        if (mmap->type == 1) {
                            total_mem += (unsigned)(mmap->len >> 32)
                                       + (unsigned)(mmap->len & 0xffffffff);
                        }
                    }
                    break;
                }
            default: break;
        }
    }
    printk("finished finding total memory: %uB\n", total_mem);
    return (total_mem / 1E6);
}
