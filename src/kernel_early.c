#include "multiboot2.h"
#include <stdint.h>

volatile uint32_t g_saved_mbi_addr;

extern void       jump_to_higher_half(void);

void              kernel_early_main(uint32_t magic, uint32_t mb_info_addr)
{
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        return;
    }
    if (mb_info_addr & 7) {
        return;
    }

    g_saved_mbi_addr = mb_info_addr;
    jump_to_higher_half();
}
