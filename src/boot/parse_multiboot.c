#include <multiboot2.h>
#include <multiboot2_info.h>
#include <stddef.h>

typedef struct s_mmap_info {
    uint32_t low_len;
    uint32_t base_low;
    uint32_t high_len;
    uint32_t base_high;
    uint8_t  type;
} __attribute__((packed)) mmap_info_t;

typedef struct s_boot_info {
    uint32_t    max_memory;
    uint8_t     mmap_max_size;
    mmap_info_t mmap_info[32];
} __attribute__((packed)) boot_info_t;

// Global instance to hold data
static boot_info_t        g_boot_info;
boot_info_t              *parse_multiboot(uint32_t magic, uint32_t mbi_addr)
{
    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        return NULL; // Error: Not booted by a compliant loader
    }
    if(mbi_addr & 7) {
        return NULL; // Error: Miss aligned pointer
    }
    uint8_t  mmap_count = 0;
    uint32_t total_mem = 0;
    uint32_t total_size = *(uint32_t *)mbi_addr; // size of the mbi
    uint8_t *tag_ptr = (uint8_t *)(mbi_addr + 8);

    while(1) {
        struct multiboot_tag *tag = (struct multiboot_tag *)tag_ptr;

        if(tag->type == MULTIBOOT_TAG_TYPE_END) {
            break;
        }

        switch(tag->type) {
            // Parse mmap to get memory information about the system this will,
            // only take the available memory for the moment this will
            // TODO: Handle other DMA available device.
            case MULTIBOOT_TAG_TYPE_MMAP:
                {
                    multiboot_memory_map_t *mmap;
                    for(mmap = ((struct multiboot_tag_mmap *)tag)->entries;
                        (multiboot_uint8_t *)mmap
                        < (multiboot_uint8_t *)tag + tag->size;
                        mmap = (multiboot_memory_map_t
                                    *)((unsigned long)mmap
                                       + ((struct multiboot_tag_mmap *)tag)
                                             ->entry_size)) {
                        if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
                            total_mem += (unsigned)(mmap->len >> 32)
                                       + (unsigned)(mmap->len & 0xffffffff);
                            g_boot_info.mmap_info[mmap_count].base_high
                                = (unsigned)(mmap->addr >> 32);
                            g_boot_info.mmap_info[mmap_count].base_low
                                = (unsigned)(mmap->addr & 0xffffffff);
                            g_boot_info.mmap_info[mmap_count].low_len
                                = (unsigned)(mmap->len >> 32);
                            g_boot_info.mmap_info[mmap_count].high_len
                                = (unsigned)(mmap->len & 0xffffffff);
                            g_boot_info.mmap_info[mmap_count].type
                                = (unsigned)(mmap->type);
                        }
                    }
                    break;
                }
            default: break;
        }
        // Move to next tag. Tags are 8-byte aligned.
        // ((size + 7) & ~7) rounds up to nearest multiple of 8
        uint32_t next_off = (tag->size + 7) & ~7;
        tag_ptr += next_off;

        // Avoid OOB and break condition.
        if((uint32_t)tag_ptr >= mbi_addr + total_size) {
            break;
        }
    }
    g_boot_info.mmap_max_size = mmap_count;
    g_boot_info.max_memory = total_mem;
    return &g_boot_info;
}
