#ifndef DEBUG_H
#define DEBUG_H

#include "../lib/io.h"
#include "../memory/mmu.h"
#include <stdint.h>

/**
 * mmu_debug_print_directory - Print the current page directory addresses
 *
 * Reports the virtual and physical address of the kernel page directory.
 */
void mmu_debug_print_directory(void);

/**
 * mmu_debug_print_page_table - Print page table location for a virtual address
 * @virt_addr: Virtual address to inspect
 *
 * If a page table exists for the given address, prints its directory index,
 * virtual address, and physical address; otherwise reports absence.
 */
void mmu_debug_print_page_table(uint32_t virt_addr);

/**
 * mmu_debug_print_pte - Print page table entry details for a virtual address
 * @virt_addr: Virtual address to inspect
 *
 * If a PTE is present, prints the frame number, flags, and raw value;
 * otherwise reports absence.
 */
void mmu_debug_print_pte(uint32_t virt_addr);

/**
 * mmu_debug_print_counters - Print MMU allocation statistics
 *
 * Outputs the total number of page directories and page tables allocated.
 */
void mmu_debug_print_counters(void);

#endif