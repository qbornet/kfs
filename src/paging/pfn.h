#ifndef PFN_H
#define PFN_H
#include <stddef.h>
#include <stdint.h>

#define PAGE_SHIFT      12 // Num of bits for 4096 (4 KiB) pages.
#define PFN_FLAG_FREE   0x00
#define PFN_FLAG_USED   0x01
#define PFN_FLAG_KERNEL 0x02
#define PFN_FLAG_DIRTY  0x04

typedef struct {
    uint32_t flags;
    uint32_t alloc_size;
} pfn_t;

pfn_t        *get_page_info(uint32_t phys_addr);
uint32_t      get_phys_addr(pfn_t *page);
void          init_pfn_db(uint32_t total_ram_size);
extern pfn_t *g_mem_map;
#endif
