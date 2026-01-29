#include <paging/pfn.h>

extern uint32_t g_kernel_end;
extern uint32_t g_total_pages;

pfn_t          *g_mem_map = NULL;

void            init_pfn_db(uint32_t total_ram_size)
{
    uint32_t total_pages = total_ram_size / 4096;

    g_mem_map = (pfn_t *)&g_kernel_end;
    uint32_t db_size_bytes = total_pages * sizeof(pfn_t);
    uint8_t *ptr = (uint8_t *)g_mem_map;
    for (uint32_t i = 0; i < db_size_bytes; i++) {
        ptr[i] = 0;
    }
}

pfn_t *get_page_info(uint32_t phys_addr)
{
    uint32_t pfn_index = phys_addr >> PAGE_SHIFT;
    if (pfn_index >= g_total_pages) return NULL;
    return &g_mem_map[pfn_index];
}

uint32_t get_phys_addr(pfn_t *page)
{
    uint32_t index = page - g_mem_map;
    return index << PAGE_SHIFT;
}
