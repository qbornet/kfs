#include <boot/kernel_early.h>
#include <boot/parser_mutliboot.h>

boot_info_t *g_boot_info_ptr;
void         kernel_early_main(uint32_t magic, uint32_t mb_info_addr)
{
    uint32_t mbi_size = *(uint32_t *)mb_info_addr;
    if (mbi_size > MULTIBOOT_SEARCH) {
        // Error: too big (handle as needed, e.g., panic or truncate)
        return;
    }
    g_boot_info_ptr = parse_multiboot(magic, mb_info_addr);
    if (g_boot_info_ptr == 0) {
        // Error magic number or mbi address not properly aligned.
        return;
    }
    jump_to_higher_half();
}
