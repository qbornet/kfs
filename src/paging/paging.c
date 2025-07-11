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

/* flush translation lookaside buffer (tlb) meaning that entry inside tlb cache,
 * (store pte) will be flushed so not saved. This avoid filling,
 * to much of the cache memory and allow us better control over the MMU caching,
 * system.
 *
 * @vaddr: Virtual Address
 * */
static __always_inline void flush_tlb(uint32_t vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr)
                 : "memory");
}

// set cr3 page directory in register
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

static __always_inline uint32_t
get_page_table_offset_address(page_table_entry_t *addr)
{
    return (((uint32_t)addr >> 12) & 0x3FF);
}

int get_free_page_table()
{
    for (int i = 0; i < 1024; i++) {
        if ((!g_page_table[i].present && !g_page_table[i].accessed)
            || !g_page_table[i].accessed)
            return i;
    }
    return -1;
}

int get_free_page_directory()
{
    for (uint32_t i = 0; i < 1024; i++) {

        // give page directory entry for only not present in memory and not
        // accessed (dirty) entry or only not accessed page directory entry.
        if ((!g_page_directory[i].present && !g_page_directory[i].accessed)
            || !g_page_directory[i].accessed)
            return i;
    }
    return -1;
}

uint32_t *get_virtual_address(void)
{
    int pdindex = get_free_page_directory();
    if (pdindex == -1) return NULL;

    // Set page directory to present so that it's available in memory
    g_page_directory[pdindex].present = 1;

    int ptindex = get_free_page_table();
    if (ptindex == -1) {
        g_page_directory[pdindex].present = 0;
        return NULL;
    }

    // Set access to indacte that it's already been use as linear adress.
    // This need to be done once all element are done being fetch.
    g_page_directory[pdindex].accessed = 1;

    // Set access to indacte that it's already been use as linear adress.
    g_page_table[ptindex].accessed = 1;

    // offset of page frame is 0 because not mapped yet.
    uint32_t vaddr = pdindex << 22 | ptindex << 12 | 0;

    // set page_table physical address to page directory.
    g_page_directory[pdindex].address
        = get_page_table_offset_address(&g_page_table[ptindex]);

    // virtual address is ready
    // and now it need to map page frame (physical addr page)
    return (uint32_t *)vaddr;
}

page_table_entry_t *get_page_table_entry(void *vaddr)
{
    uint32_t pte_index = PTE_INDEX((uintptr_t)vaddr);
    return &g_page_table[pte_index];
}

page_directory_entry_t *get_page_directory_entry(void *vaddr)
{
    uint32_t pde_index = PDE_INDEX((uintptr_t)vaddr);
    return &g_page_directory[pde_index];
}

uint32_t get_address_value(void *phys)
{
    return (uint32_t)phys >> 12;
}

void setup_identity_paging(void)
{
    memset(g_page_directory, 0, sizeof(g_page_directory));
    memset(g_page_table, 0, sizeof(g_page_table));

    g_page_directory[KERNEL_PAGE_DIRECTORY_INDEX].present = 1;
    g_page_directory[KERNEL_PAGE_DIRECTORY_INDEX].read_write = 1;
    g_page_directory[KERNEL_PAGE_DIRECTORY_INDEX].user_supervisor = 0;
    g_page_directory[KERNEL_PAGE_DIRECTORY_INDEX].address
        = ((uint32_t)g_page_table) >> 12;
    for (uint32_t i = 0; i < 1024; i++) {
        g_page_table[i].present = 1;
        g_page_table[i].read_write = 1;
        g_page_table[i].user_supervisor = 0;
        g_page_table[i].address = i;
    }
}

void init_paging(uint32_t mem_in_mib)
{
    uint32_t max_mem = 0x100000 * mem_in_mib;
    printk("Memory available: %luMiB, in byte: %luB\n", mem_in_mib, max_mem);
    printk("start identity paging\n");
    setup_identity_paging();
    printk("finished identity paging\n");
    set_cr3((uintptr_t)&g_page_directory[KERNEL_PAGE_DIRECTORY_INDEX]);
    set_cr0(1 << 31);
}
