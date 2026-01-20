#include <gdt.h>
#include <lib/io.h>
#include <lib/mem.h>
#include <multiboot2.h>
#include <stdint.h>

volatile uint32_t g_saved_mbi_addr;
char              g_mbi_copy[MULTIBOOT_SEARCH];

extern void       jump_to_higher_half(void);

void              kernel_early_main(uint32_t magic, uint32_t mb_info_addr)
{
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        return;
    }
    if (mb_info_addr & 7) {
        return;
    }

    uint32_t mbi_size = *(uint32_t *)mb_info_addr;
    if (mbi_size > MULTIBOOT_SEARCH) {
        // Error: too big (handle as needed, e.g., panic or truncate)
        return;
    }
    memcpy(g_mbi_copy, (void *)mb_info_addr, mbi_size);

    // Save high virutal address of the copy
    g_saved_mbi_addr = (uint32_t)g_mbi_copy;
    jump_to_higher_half();
}
