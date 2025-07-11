#include "multiboot2_info.h"

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
    uint32_t              total_mem;
    for (tag = (struct multiboot_tag *)(mbi + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag
                                        + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                {
                    total_mem
                        = ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower
                        + ((struct multiboot_tag_basic_meminfo *)tag)
                              ->mem_upper;
                    break;
                }
            default: break;
        }
    }
    return total_mem / 1049;
}
