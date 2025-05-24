#include "kmalloc.h"

// End of kernel image (set in linker.ld)
extern uint32_t g_end;

// Bump-pointer allocator state
static uint32_t g_heap_ptr = 0;

/**
 * bump_init_heap - Initialize heap start address
 */
static void     bump_init_heap(void)
{
    g_heap_ptr = (uint32_t)&g_end;
}

/**
 * kmalloc - Allocate memory from heap
 * @size: Number of bytes to allocate
 * Returns aligned pointer or NULL on failure
 */
void *kmalloc(size_t size)
{
    if(!g_heap_ptr) bump_init_heap();

    // Align pointer to machine word
    uint32_t aligned
        = (g_heap_ptr + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
    void *addr = (void *)aligned;

    // Advance heap pointer (ensure at least one word)
    g_heap_ptr = aligned + (size > 0 ? size : sizeof(void *));
    return addr;
}

/**
 * kmalloc_aligned - Allocate page-aligned memory
 * @size: Number of bytes to allocate
 * Returns page-aligned pointer
 */
void *kmalloc_aligned(size_t size)
{
    if(!g_heap_ptr) bump_init_heap();

    // Align pointer to page boundary
    uint32_t aligned = (g_heap_ptr + PAGE_SIZE - 1) & PAGE_ALIGN_MASK;
    void    *addr = (void *)aligned;

    g_heap_ptr = aligned + size;
    return addr;
}