#ifndef PAGING_H
#define PAGING_H

#include <lib/io.h>
#include <lib/mem.h>
#include <lib/shared/defs.h>
#include <paging/phys_alloc.h>
#include <stdbool.h>
#include <stdint.h>

// For page directory and table present in memory is ON or OFF
#define PF_PRESENT_ON               1
#define PF_PRESENT_OFF              0

// For page directory and table Read only ON, Read & Write OFF
#define PF_RW_ON                    1
#define PF_RW_OFF                   0

// For page directory and table user (RING0) OFF, user is (RING>1) ON
#define PF_USER_ON                  1
#define PF_USER_OFF                 0

#define PTE_INDEX(addr)             (((uint32_t)addr >> 12) & 0x3FF)
#define PDE_INDEX(addr)             (((uint32_t)addr >> 22) & 0x3FF)

#define VIRTUAL_BASE                0xC0000000
#define KERNEL_START_VIRT_MEM       0xC0400000
#define USER_START_VIRT_MEM         0x00400000
#define KERNEL_MAP_SIZE             0x00400000
#define KERNEL_OFFSET               0xBFF00000
#define V2P(a)                      (((uint32_t)(a) - KERNEL_OFFSET))
#define P2V(a)                      (((uint32_t)(a) + KERNEL_OFFSET))

// Kernel maping start at 768 in page directory end at 1024
#define KERNEL_PAGE_DIRECTORY_INDEX 768

// User maping start at 0 in page directory end at 768 (Kernel space)
#define USER_PAGE_DIRECTORY_INDEX   1

/*
 * Page directory entry point to page table entry and define page table entry
 * rights
 * */
typedef struct s_page_directory_entry {
    uint8_t  present : 1;
    uint8_t  rw : 1;
    uint8_t  user : 1;
    uint8_t  pwt : 1;
    uint8_t  cache_disable : 1;
    uint8_t  accessed : 1;
    uint16_t ignored : 6;
    uint32_t address : 20;
} __attribute__((packed)) page_directory_entry_t, pde_t;

/*
 * Page table entry use to define rights and point to page
 * */
typedef struct s_page_table_entry {
    uint8_t  present : 1;
    uint8_t  rw : 1;
    uint8_t  user : 1;
    uint8_t  pwt : 1;
    uint8_t  cache_disable : 1;
    uint8_t  accessed : 1;
    uint8_t  dirty : 1;
    uint16_t ignored : 5;
    uint32_t address : 20;
} __attribute__((packed)) page_table_entry_t, pte_t;

// Init paging
void                      destroy_memory_page(void *vaddr);
void *get_memory_page(page_frame_t frame, uint32_t vaddr, uint8_t flags);
void  init_paging(uint32_t base_addrs, uint32_t size);

extern uint32_t g_page_table_kernel_space[];
extern uint32_t g_page_table_user_space[];
extern uint32_t g_page_directory[];
extern uint32_t g_kernel_end;
extern uint32_t g_kernel_start_virt_mem;
extern uint32_t g_user_start_virt_mem;
#endif
