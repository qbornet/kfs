#include "tests.h"

#define ASSERT(expr)                       \
    do {                                   \
        if(!(expr)) {                      \
            printk("FAILED: %s\n", #expr); \
            return -1;                     \
        }                                  \
    } while(0)

/**
 * test_identity_map - Verify identity mapping for low memory
 * Returns 0 on success, -1 on failure.
 */
int test_identity_map(void)
{
    uint32_t v = 0x00102000;
    pte_t   *pte = mmu_get_pte(v, false);
    ASSERT(pte && (*pte & PF_PRESENT));
    uint32_t phys = *pte & PAGE_ALIGN_MASK;
    ASSERT(phys == v);
    return 0;
}

/**
 * test_map_unmap - Test mapping and unmapping of a single page
 */
int test_map_unmap(void)
{
    uint32_t va = 0x00400000;
    mmu_map_page(va, 0xABC000, PF_PRESENT | PF_RW);

    pte_t *pte = mmu_get_pte(va, false);
    ASSERT(pte && (*pte & PF_PRESENT));
    ASSERT((*pte & PAGE_ALIGN_MASK) == 0xABC000);

    mmu_unmap_page(va);
    pte = mmu_get_pte(va, false);
    ASSERT(!pte || !(*pte & PF_PRESENT));
    return 0;
}

/**
 * test_access_dirty_bits - Ensure WS-Clock tracks accessed and dirty bits
 */
int test_access_dirty_bits(void)
{
    uint32_t va = 0x00500000;
    mmu_map_page(va, 0, PF_PRESENT | PF_RW);
    pte_t *pte = mmu_get_pte(va, false);
    ASSERT(pte);

    *pte |= PF_DIRTY | PF_ACCESSED;
    wsclock_maintenance();

    ASSERT(!(*pte & PF_ACCESSED));

    extern ws_frame_t g_ws_clock[];
    int               frame = (*pte & PAGE_ALIGN_MASK) >> 12;
    ASSERT(g_ws_clock[frame].dirty);
    return 0;
}

/**
 * test_wsclock_evict - Force immediate WS-Clock eviction
 */
int test_wsclock_eviction(void)
{
    extern uint32_t g_ws_tau;
    g_ws_tau = 0;

    mmu_map_page(0x00600000, 0, PF_PRESENT | PF_RW);
    mmu_map_page(0x00700000, 0, PF_PRESENT | PF_RW);

    int victim = wsclock_evict_frame();
    ASSERT(victim >= 0);
    return 0;
}

/**
 * test_pte_uniqueness - Check distinct PTEs for different VAs
 */
int test_pte_uniqueness(void)
{
    pte_t *pte1 = mmu_get_pte(0x1000, true);
    pte_t *pte2 = mmu_get_pte(0x2000, true);
    ASSERT(pte1 && pte2);
    ASSERT(pte1 != pte2);
    return 0;
}

/**
 * test_remap_overwrite - Verify remapping overwrites existing PTE
 */
int test_remap_overwrite(void)
{
    const uint32_t va = 0x3000;
    mmu_map_page(va, 0xAAA000, PF_PRESENT | PF_RW);
    pte_t *pte = mmu_get_pte(va, false);
    ASSERT((*pte & PAGE_ALIGN_MASK) == 0xAAA000);

    mmu_map_page(va, 0xBBB000, PF_PRESENT | PF_RW);
    ASSERT((*pte & PAGE_ALIGN_MASK) == 0xBBB000);
    return 0;
}

/**
 * test_switch_directory_isolation - Ensure directory switching isolates
 * mappings
 */
int test_switch_directory_isolation(void)
{
    const uint32_t    va = 0x10000000;

    page_directory_t *new_dir = kmalloc_aligned(sizeof(*new_dir));
    memset(new_dir, 0, sizeof(*new_dir));

    g_current_directory = new_dir;
    mmu_map_page(va, 0xDEAD000, PF_PRESENT | PF_RW);
    pte_t *p_new = mmu_get_pte(va, false);
    ASSERT(p_new && ((*p_new & PAGE_ALIGN_MASK) == 0xDEAD000));

    mmu_switch_directory(g_kernel_directory);
    pte_t *p_old = mmu_get_pte(va, false);
    ASSERT(!p_old || !(*p_old & PF_PRESENT));
    return 0;
}

/**
 * test_kmalloc_basic - Basic kmalloc monotonicity and non-null
 */
int test_kmalloc_basic(void)
{
    void *p1 = kmalloc(16);
    ASSERT(p1);
    void *p2 = kmalloc(32);
    ASSERT(p2);
    ASSERT((uintptr_t)p2 > (uintptr_t)p1);
    return 0;
}

/**
 * test_kmalloc_aligned - Ensure kmalloc_aligned returns PAGE_SIZE boundary
 */
int test_kmalloc_aligned(void)
{
    void *p = kmalloc_aligned(100);
    ASSERT(p);
    ASSERT(((uintptr_t)p & (PAGE_SIZE - 1)) == 0);
    return 0;
}

/**
 * test_kmalloc_zero - Zero-size allocations still advance pointer
 */
int test_kmalloc_zero(void)
{
    void *p1 = kmalloc(0);
    ASSERT(p1);
    void *p2 = kmalloc(0);
    ASSERT(p2);
    ASSERT((uintptr_t)p2 > (uintptr_t)p1);
    return 0;
}

/**
 * test_kmalloc_multiple - Multiple small allocations preserve alignment
 */
int test_kmalloc_multiple(void)
{
    void *base = kmalloc(8);
    ASSERT(base);
    void *next = kmalloc(8);
    ASSERT(next);
    uintptr_t diff = (uintptr_t)next - (uintptr_t)base;
    ASSERT(diff >= 8);
    ASSERT(((uintptr_t)base & (sizeof(void *) - 1)) == 0);
    return 0;
}