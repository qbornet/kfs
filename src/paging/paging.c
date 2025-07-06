#include "paging.h"

page_directory_entry_t g_page_directory[1024] __attribute__((aligned(4096)));
page_table_entry_t     g_page_table[1024] __attribute__((aligned(4096)));

/*
static bool            is_physical_addr_aligned(void *physical_addr)
{
    uintptr_t to_check = (uintptr_t)physical_addr;

    // check if last 12 bits are 0 return true if so.
    if (!((to_check >> 12) & 0xfff)) {
        return true;
    }
    return false;
}
*/

static __always_inline void set_cr3(uintptr_t page_directory)
{
    asm volatile(".intel_syntax noprefix\n\t"
                 "mov eax, %0\n\t"
                 "mov cr3, eax\n\t"
                 ".att_syntax prefix\n\t" ::"r"(page_directory)
                 : "eax");
}

// Inline asm to set CR0 for paging.
static __always_inline void set_cr0(uint32_t value)
{
    asm volatile(".intel_syntax noprefix\n\t"
                 "mov eax, cr0\n\t"
                 "or eax, %0\n\t"
                 "mov cr0, eax\n\t"
                 ".att_syntax prefix\n\t"
                 :
                 : "r"(value)
                 : "eax");
}

uint32_t get_page_table_phys_address(page_table_entry_t *addr)
{
    return ((uint32_t)addr >> 12);
}

void init_paging(void)
{
    for (int i = 0; i < 1024; i++) {
        g_page_directory[i].read_write = 1;
        g_page_table[i].present = 1;
        g_page_table[i].read_write = 1;
        g_page_table[i].address = i;
    }
    g_page_directory[0].present = 1;
    g_page_directory[0].address = get_page_table_phys_address(g_page_table);
    printk("Init paging\n");
    set_cr3((uintptr_t)g_page_directory);
    set_cr0(1 << 31);
}
