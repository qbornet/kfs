#ifndef TESTS_H
#define TESTS_H

#include "../debug/debug.h"
#include "../lib/kmalloc.h"
#include "../lib/printk.h"
#include "../memory/mmu.h"
#include <stdint.h>

/**
 * test_identity_map - Check identity mapping for low memory range
 * Returns 0 on success, negative on failure.
 */
int test_identity_map(void);

/**
 * test_map_unmap - Validate mapping and unmapping of a page
 */
int test_map_unmap(void);

/**
 * test_access_dirty_bits - Verify WS-Clock tracks accessed and dirty flags
 */
int test_access_dirty_bits(void);

/**
 * test_wsclock_eviction - Ensure WS-Clock eviction occurs under pressure
 */
int test_wsclock_eviction(void);

/**
 * test_pte_uniqueness - Ensure distinct PTEs for distinct virtual addresses
 */
int test_pte_uniqueness(void);

/**
 * test_remap_overwrite - Confirm remapping overwrites existing PTE correctly
 */
int test_remap_overwrite(void);

/**
 * test_switch_directory_isolation - Verify page directory switching isolates
 * mappings
 */
int test_switch_directory_isolation(void);

/**
 * test_kmalloc_basic - Test basic kmalloc: non-null and monotonic
 */
int test_kmalloc_basic(void);

/**
 * test_kmalloc_aligned - Test kmalloc_aligned returns page-aligned address
 */
int test_kmalloc_aligned(void);

/**
 * test_kmalloc_zero - Test zero-size kmalloc allocations advance pointer
 */
int test_kmalloc_zero(void);

/**
 * test_kmalloc_multiple - Test sequential small allocations preserve alignment
 */
int test_kmalloc_multiple(void);

#endif