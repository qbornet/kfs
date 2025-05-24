#include "debug.h"

extern uint32_t g_debug_dir_count;
extern uint32_t g_debug_pt_count;

/**
 * mmu_debug_print_directory - Print addresses of current page directory
 * Reports the virtual and physical address of the kernel directory.
 */
void            mmu_debug_print_directory(void)
{
    printk("MMU: page directory @ virt=%p phys=%p\n",
           g_kernel_directory,
           (void *)(uintptr_t)g_kernel_directory);
}

/**
 * mmu_debug_print_page_table - Print location of page table for a VA
 * @virt_addr: Virtual address to inspect
 * Reports if no table present, otherwise prints its indices and addresses.
 */
void mmu_debug_print_page_table(uint32_t virt_addr)
{
    uint32_t pd_idx = PDE_INDEX(virt_addr);
    pde_t    entry = g_current_directory->entries[pd_idx];

    if(!(entry & PF_PRESENT)) {
        printk("MMU: no page-table for VA %p (dir index %lu)\n",
               (void *)(uintptr_t)virt_addr,
               (unsigned long)pd_idx);
        return;
    }

    uint32_t phys = entry & PAGE_ALIGN_MASK;
    void    *virt = (void *)(uintptr_t)phys;
    printk("MMU: page-table[%lu] @ virt=%p phys=%p\n",
           (unsigned long)pd_idx,
           virt,
           (void *)(uintptr_t)phys);
}

/**
 * mmu_debug_print_pte - Print PTE details for a VA
 * @virt_addr: Virtual address to inspect
 * Reports if no PTE present, otherwise prints frame number and flags.
 */
void mmu_debug_print_pte(uint32_t virt_addr)
{
    pte_t *pte = mmu_get_pte(virt_addr, false);

    if(!pte || !(*pte & PF_PRESENT)) {
        printk("MMU: no PTE for VA %p\n", (void *)(uintptr_t)virt_addr);
        return;
    }

    uint32_t      val = *pte;
    unsigned long frame = (val & PAGE_ALIGN_MASK) >> 12;
    unsigned long flags = val & ~PAGE_ALIGN_MASK;

    printk("MMU: PTE for VA %p -> frame=%lu flags=0x%lX raw=0x%lX\n",
           (void *)(uintptr_t)virt_addr,
           frame,
           flags,
           (unsigned long)val);
}

/**
 * mmu_debug_print_counters - Print number of directories and tables
 */
void mmu_debug_print_counters(void)
{
    printk("MMU: directories allocated=%lu, tables allocated=%lu\n",
           (unsigned long)g_debug_dir_count,
           (unsigned long)g_debug_pt_count);
}
