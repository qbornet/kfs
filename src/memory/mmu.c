#include "mmu.h"

// Debug counters for allocated page directories and tables
uint32_t          g_debug_dir_count = 0;
uint32_t          g_debug_pt_count = 0;

// Bitmap tracking physical frame usage
static uint32_t  *g_frame_bitmap;
// Total physical frame count
static uint32_t   g_total_frames;

// Current and kernel page directories
page_directory_t *g_kernel_directory = NULL;
page_directory_t *g_current_directory = NULL;

// WS-Clock (working set) data structures
ws_frame_t        g_ws_clock[MAX_FRAMES]; // Circular frame queue
int               g_ws_hand = 0;          // Clock hand pointer
uint32_t          g_ws_tau = 100;         // Working-set window (ticks)
int               g_ws_total = 0;         // Frames currently in use

// Return system ticks (to be implemented with PIT/TSC)
uint32_t          timer_get_ticks(void)
{
    return 0;
}

// Helper: get current tick count
static inline uint32_t now_ticks(void)
{
    return timer_get_ticks();
}

// Entry for pending write-back of dirty pages
typedef struct {
    uint32_t virt_addr;
    uint32_t phys_addr;
} wb_entry_t;

static wb_entry_t g_wb_queue[WB_QUEUE_SIZE];
static int        g_wb_head = 0;
static int        g_wb_tail = 0;

/**
 * swap_write_page - Write a dirty page back to storage
 * @virt_addr: Virtual address of the page
 * @phys_addr: Physical frame address
 * (Currently a stub; implement block device or swap I/O here.)
 */
void              swap_write_page(uint32_t virt_addr, uint32_t phys_addr)
{
    (void)virt_addr;
    (void)phys_addr;
}

/**
 * schedule_writeback - Enqueue a dirty page for deferred write-back
 * @virt_addr: Virtual address of the page
 * @phys_addr: Physical frame address
 */
static void schedule_writeback(uint32_t virt_addr, uint32_t phys_addr)
{
    int next = (g_wb_tail + 1) % WB_QUEUE_SIZE;
    if(next == g_wb_head) {
        // Overwrite oldest entry if queue is full
        g_wb_head = (g_wb_head + 1) % WB_QUEUE_SIZE;
    }
    g_wb_queue[g_wb_tail].virt_addr = virt_addr;
    g_wb_queue[g_wb_tail].phys_addr = phys_addr;
    g_wb_tail = next;
}

/**
 * process_writeback_queue - Flush all pending dirty pages immediately
 */
void process_writeback_queue(void)
{
    while(g_wb_head != g_wb_tail) {
        wb_entry_t e = g_wb_queue[g_wb_head];
        swap_write_page(e.virt_addr, e.phys_addr);
        g_wb_head = (g_wb_head + 1) % WB_QUEUE_SIZE;
    }
}

/**
 * alloc_frame_ws - Allocate a free frame and add it to the WS-Clock
 * @virt_addr: Virtual address for this frame
 * Returns frame index or -1 on failure
 */
static int alloc_frame_ws(uint32_t virt_addr)
{
    int32_t  frame = -1;
    uint32_t map_len = (g_total_frames + 31) >> 5;

    // Find first zero bit in frame bitmap
    for(uint32_t i = 0; i < map_len; ++i) {
        if(g_frame_bitmap[i] != 0xFFFFFFFFU) {
            for(uint32_t bit = 0; bit < 32; ++bit) {
                if(!(g_frame_bitmap[i] & (1U << bit))) {
                    frame = (i << 5) + bit;
                    break;
                }
            }
            if(frame >= 0) break;
        }
    }
    if(frame < 0 || (uint32_t)frame >= g_total_frames) return -1;

    // Mark frame used and initialize WS entry
    g_frame_bitmap[frame >> 5] |= (1U << (frame & 31));
    g_ws_clock[frame].phys_addr = (uint32_t)frame * PAGE_SIZE;
    g_ws_clock[frame].virt_addr = virt_addr;
    g_ws_clock[frame].last_used = now_ticks();
    g_ws_clock[frame].dirty = false;
    g_ws_clock[frame].in_use = true;
    g_ws_total++;

    return frame;
}

/**
 * free_frame_ws - Remove a frame from WS-Clock and mark it free
 * @frame: Index of frame to free
 */
static void free_frame_ws(int frame)
{
    if(frame < 0 || (uint32_t)frame >= g_total_frames) return;

    g_frame_bitmap[frame >> 5] &= ~(1U << (frame & 31));
    g_ws_clock[frame].in_use = false;
    g_ws_total--;
}

/**
 * wsclock_evict_frame - Evict a page via WS-Clock algorithm
 * Returns victim frame index or -1 if no frames to evict
 */
int wsclock_evict_frame(void)
{
    if(g_ws_total == 0) return -1;

    int      start = g_ws_hand;
    uint32_t now = now_ticks();

    do {
        ws_frame_t *f = &g_ws_clock[g_ws_hand];
        if(f->in_use) {
            pte_t *pte = mmu_get_pte(f->virt_addr, false);
            if(pte && (*pte & PF_PRESENT)) {
                if(*pte & PF_ACCESSED) {
                    // Recently used: clear accessed bit and update time
                    f->last_used = now;
                    *pte &= ~PF_ACCESSED;
                } else {
                    uint32_t age = now - f->last_used;
                    if(!f->dirty && age > g_ws_tau) break;
                    if(f->dirty) {
                        schedule_writeback(f->virt_addr, f->phys_addr);
                        f->dirty = false;
                        *pte &= ~PF_DIRTY;
                    } else {
                        break;
                    }
                }
            }
        }
        g_ws_hand = (g_ws_hand + 1) % g_total_frames;
    } while(g_ws_hand != start);

    int         victim = g_ws_hand;
    ws_frame_t *v = &g_ws_clock[victim];

    // Remove page table entry and flush TLB
    pte_t      *vpte = mmu_get_pte(v->virt_addr, false);
    if(vpte) {
        *vpte = 0;
        asm volatile("invlpg (%0)"
                     :
                     : "r"(v->virt_addr)
                     : "memory");
    }

    free_frame_ws(victim);
    g_ws_hand = (victim + 1) % g_total_frames;
    return victim;
}

/**
 * wsclock_maintenance - Periodic maintenance of WS-Clock bits
 * Clears accessed bits and updates dirty status
 */
void wsclock_maintenance(void)
{
    for(int i = 0; i < (int)g_total_frames; ++i) {
        if(!g_ws_clock[i].in_use) continue;

        pte_t *pte = mmu_get_pte(g_ws_clock[i].virt_addr, false);
        if(!pte) continue;

        if(*pte & PF_ACCESSED) {
            g_ws_clock[i].last_used = now_ticks();
            *pte &= ~PF_ACCESSED;
        }
        if(*pte & PF_DIRTY) {
            g_ws_clock[i].dirty = true;
        }
    }
}

/**
 * mmu_init - Initialize MMU structures and identity-map physical memory
 * @mem_end: End of physical memory to map
 */
void mmu_init(uint32_t mem_end)
{
    g_total_frames = mem_end / PAGE_SIZE;
    uint32_t bm_size = ((g_total_frames + 31) >> 5) * sizeof(uint32_t);
    g_frame_bitmap = (uint32_t *)kmalloc_aligned(bm_size);
    memset(g_frame_bitmap, 0, bm_size);

    for(uint32_t i = 0; i < g_total_frames; ++i) g_ws_clock[i].in_use = false;
    g_ws_hand = 0;
    g_ws_total = 0;

    g_kernel_directory
        = (page_directory_t *)kmalloc_aligned(sizeof(page_directory_t));
    memset(g_kernel_directory, 0, sizeof(page_directory_t));
    g_current_directory = g_kernel_directory;

    // Identity-map physical memory pages
    for(uint32_t addr = 0; addr < mem_end; addr += PAGE_SIZE) {
        pte_t *pte = mmu_get_pte(addr, true);
        if(!pte) continue;
        *pte = (addr & PAGE_ALIGN_MASK) | PF_PRESENT | PF_RW;
        asm volatile("invlpg (%0)"
                     :
                     : "r"(addr)
                     : "memory");
    }

    // Load page directory into CR3
    mmu_switch_directory(g_kernel_directory);
}

/**
 * mmu_get_pte - Retrieve or create a page-table entry for a virtual address
 * @virt_addr: Virtual address to translate
 * @create: Create table if missing
 * Returns pointer to PTE or NULL
 */
pte_t *mmu_get_pte(uint32_t virt_addr, bool create)
{
    uint32_t      pd_idx = PDE_INDEX(virt_addr);
    pde_t         pde = g_current_directory->entries[pd_idx];
    page_table_t *table;

    if(pde & PF_PRESENT) {
        table = (page_table_t *)(pde & PAGE_ALIGN_MASK);
    } else if(create) {
        table = (page_table_t *)kmalloc_aligned(sizeof(page_table_t));
        memset(table, 0, sizeof(page_table_t));
        g_current_directory->entries[pd_idx]
            = ((uint32_t)table & PAGE_ALIGN_MASK) | PF_PRESENT | PF_RW
            | PF_USER;
    } else {
        return NULL;
    }

    uint32_t pt_idx = PTE_INDEX(virt_addr);
    return &table->entries[pt_idx];
}

/**
 * mmu_map_page - Map or allocate a physical frame for a virtual address
 * @virt_addr: Virtual address to map
 * @phys_addr: Physical address or 0 to allocate
 * @flags: Page flags (present, RW, user)
 */
void mmu_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags)
{
    pte_t *pte = mmu_get_pte(virt_addr, true);
    if(!pte) return;

    if(phys_addr == 0) {
        int frame = alloc_frame_ws(virt_addr);
        if(frame < 0) frame = wsclock_evict_frame();
        if(frame < 0) return;
        phys_addr = (uint32_t)frame * PAGE_SIZE;
    }

    *pte = (phys_addr & PAGE_ALIGN_MASK) | (flags & ~PAGE_ALIGN_MASK);
    asm volatile("invlpg (%0)"
                 :
                 : "r"(virt_addr)
                 : "memory");
}

/**
 * mmu_unmap_page - Unmap a virtual address and free its frame
 * @virt_addr: Virtual address to unmap
 */
void mmu_unmap_page(uint32_t virt_addr)
{
    pte_t *pte = mmu_get_pte(virt_addr, false);
    if(!pte || !(*pte & PF_PRESENT)) return;

    uint32_t phys = *pte & PAGE_ALIGN_MASK;
    int      frame = phys >> 12;
    *pte = 0;
    asm volatile("invlpg (%0)"
                 :
                 : "r"(virt_addr)
                 : "memory");

    free_frame_ws(frame);
}

/**
 * mmu_switch_directory - Change the active page directory
 * @new_dir: Pointer to new page_directory_t
 */
void mmu_switch_directory(page_directory_t *new_dir)
{
    g_current_directory = new_dir;
    uint32_t phys = (uint32_t)new_dir;
    asm volatile("movl %0, %%cr3"
                 :
                 : "r"(phys)
                 : "memory");
}
