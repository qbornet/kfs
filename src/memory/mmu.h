#ifndef MMU_H
#define MMU_H

#include "../kernel.h"
#include "../lib/kmalloc.h"
#include "../lib/mem.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Page and table sizes
#define PAGE_SIZE          0x1000U
#define PAGE_DIR_ENTRIES   1024U
#define PAGE_TABLE_ENTRIES 1024U
#define PAGE_ALIGN_MASK    0xFFFFF000U

// Maximum number of frames for WS-Clock
#define MAX_FRAMES         1024

// Index calculations for directory and table
#define PDE_INDEX(addr)    (((addr) >> 22) & 0x3FF)
#define PTE_INDEX(addr)    (((addr) >> 12) & 0x3FF)

// Page flag bits
#define PF_PRESENT         (1U << 0)
#define PF_RW              (1U << 1)
#define PF_USER            (1U << 2)
#define PF_PWT             (1U << 3)
#define PF_PCD             (1U << 4)
#define PF_ACCESSED        (1U << 5)
#define PF_DIRTY           (1U << 6)
#define PF_SIZE_4MB        (1U << 7)
#define PF_GLOBAL          (1U << 8)

// Write-back queue size for dirty pages
#define WB_QUEUE_SIZE      128

/**
 * ws_frame_t - Working-set frame metadata
 * @virt_addr: Virtual address mapped
 * @phys_addr: Physical base address of frame
 * @last_used: Timestamp of last access reset
 * @dirty:     True if page was modified
 * @in_use:    True if frame is in WS-Clock
 */
typedef struct {
    uint32_t virt_addr;
    uint32_t phys_addr;
    uint32_t last_used;
    bool     dirty;
    bool     in_use;
} ws_frame_t;

// WS-Clock global structures
extern ws_frame_t g_ws_clock[MAX_FRAMES];
extern int        g_ws_hand;
extern uint32_t   g_ws_tau;
extern int        g_ws_total;

// Page directory and table entry types
typedef uint32_t  pde_t;
typedef uint32_t  pte_t;

/**
 * page_table_t - Single page table (aligned to page size)
 * entries: Array of page-table entries
 */
typedef struct page_table_s {
    pte_t entries[PAGE_TABLE_ENTRIES];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

/**
 * page_directory_t - Single page directory (aligned to page size)
 * entries: Array of page-directory entries
 */
typedef struct page_directory_s {
    pde_t entries[PAGE_DIR_ENTRIES];
} __attribute__((aligned(PAGE_SIZE))) page_directory_t;

// Current and kernel page directories
extern page_directory_t              *g_kernel_directory;
extern page_directory_t              *g_current_directory;

/**
 * mmu_init - Initialize frame bitmap, directory, and identity map
 * @mem_end: End of physical memory to map (bytes)
 */
void                                  mmu_init(uint32_t mem_end);

/**
 * mmu_get_pte - Get or create the PTE for a virtual address
 * @virt_addr: Virtual address
 * @create:    Allocate table if missing
 * Returns pointer to PTE or NULL
 */
pte_t *mmu_get_pte(uint32_t virt_addr, bool create);

/**
 * mmu_map_page - Map virtual to physical address with flags
 * @virt_addr: Virtual address to map
 * @phys_addr: Physical frame address (0 to allocate)
 * @flags:     Page flags (PF_*)
 */
void   mmu_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);

/**
 * mmu_unmap_page - Unmap a virtual address and free frame
 * @virt_addr: Virtual address to unmap
 */
void   mmu_unmap_page(uint32_t virt_addr);

/**
 * mmu_switch_directory - Load new page directory into CR3
 * @new_dir: Pointer to directory structure
 */
void   mmu_switch_directory(page_directory_t *new_dir);

/**
 * wsclock_evict_frame - Evict a frame using WS-Clock
 * Returns index of evicted frame or -1
 */
int    wsclock_evict_frame(void);

/**
 * wsclock_maintenance - Periodic update of accessed and dirty bits
 */
void   wsclock_maintenance(void);

#endif
