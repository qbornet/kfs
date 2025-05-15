#include "kernel_heap.h"
#include "ordered_array.h"
#include "paging.h"

extern page_directory_t *g_kernel_directory;
heap_t                  *g_kheap = 0;

/**
 * Primary allocator: before heap is initialized, uses placement address;
 * afterwards delegates to paging-based heap ('alloc').
 * If 'align' is non-zero, result is page-aligned.
 * If 'physical_address' is non-null, stores the physical address of the
 * allocation.
 */
static uint32_t __kmalloc(uint32_t size, int align, uint32_t *physical_address) //NOLINT
{
    uint32_t placement_address = (uint32_t)&g_end;

    if(g_kheap != 0) {
        void *address = alloc(size, (uint8_t)align, g_kheap);
        if(physical_address) {
            page_t *page = get_page((uint32_t)address, 0, g_kernel_directory);
            *physical_address
                = page->frame * PAGE_SIZE + ((uint32_t)address & 0xFFF);
        }
        return (uint32_t)address;
    }

    if(align && (placement_address & PAGE_ALIGN_MASK)) {
        placement_address &= PAGE_ALIGN_MASK;
        placement_address += PAGE_SIZE;
    }

    uint32_t base = placement_address;
    if(physical_address) *physical_address = base;
    placement_address += size;
    return base;
}

/**
 * Allocate 'size' bytes aligned to a page boundary.
 */
uint32_t kmalloc_aligned(uint32_t size)
{
    return __kmalloc(size, 1, 0);
}

/**
 * Allocate 'size' bytes and return the physical address via out-parameter.
 */
uint32_t kmalloc_p(uint32_t size, uint32_t *physical_address)
{
    return __kmalloc(size, 0, physical_address);
}

/**
 * Allocate 'size' bytes page-aligned, and return physical address via
 * out-parameter.
 */
uint32_t kmalloc_alignedp(uint32_t size, uint32_t *physical_address)
{
    return __kmalloc(size, 1, physical_address);
}

/**
 * Simple allocator: allocate 'size' bytes without alignment or physical
 * address.
 */
uint32_t kmalloc(uint32_t size)
{
    return __kmalloc(size, 0, 0);
}

/**
 * Search heap index for the smallest free block that can satisfy 'size',
 * taking alignment into account if 'page_align' is set.
 * Returns index in ordered array or -1 if none found.
 */
static int32_t
find_smallest_hole(uint32_t size, uint8_t page_align, heap_t *heap)
{
    int32_t i = 0;
    while(i < heap->index.size) {
        header_t *header = (header_t *)ordered_array_lookup(i, &heap->index);
        int32_t   hole_size = (int32_t)header->size;
        if(page_align) {
            uint32_t start = (uint32_t)header + sizeof(header_t);
            int32_t  offset = start & (PAGE_SIZE - 1);
            if(offset) offset = PAGE_SIZE - offset;
            hole_size -= offset;
        }
        if(hole_size
           >= (int32_t)size + (int32_t)(sizeof(header_t) + sizeof(footer_t)))
            break;
        i++;
    }
    return (i == heap->index.size) ? -1 : i;
}

/**
 * Predicate for ordered array: compares two headers by their size.
 */
static int8_t header_t_less_than(void *a, void *b)
{
    return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

/**
 * Initialize a new kernel heap at [start, end), up to max.
 * 'supervisor' and 'readonly' control page permissions.
 */
heap_t *create_heap(uint32_t start,
                    uint32_t end,
                    uint32_t max,
                    uint8_t  supervisor,
                    uint8_t  readonly)
{
    // ASSERT(start % PAGE_SIZE == 0);
    // ASSERT(end % PAGE_SIZE == 0);
    heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

    heap->index = ordered_array_place(
        (void *)start, HEAP_INDEX_SIZE, &header_t_less_than);
    start += HEAP_INDEX_SIZE * sizeof(type_t);
    if(start & (PAGE_SIZE - 1)) {
        start &= PAGE_ALIGN_MASK;
        start += PAGE_SIZE;
    }

    heap->start_address = start;
    heap->end_address = end;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    header_t *hole = (header_t *)start;
    hole->magic = HEAP_MAGIC;
    hole->size = end - start;
    hole->is_hole = 1;
    ordered_array_insert((void *)hole, &heap->index);
    return heap;
}

/**
 * Grow the heap to accommodate at least 'new_size', allocating pages as needed.
 */
static void expand(uint32_t new_size, heap_t *heap)
{
    // ASSERT(new_size > heap->end_address - heap->start_address);
    if(new_size & (PAGE_SIZE - 1)) {
        new_size &= PAGE_ALIGN_MASK;
        new_size += PAGE_SIZE;
    }
    // ASSERT(heap->start_address + new_size <= heap->max_address);
    uint32_t old_size = heap->end_address - heap->start_address;
    for(uint32_t i = old_size; i < new_size; i += PAGE_SIZE) {
        alloc_frame(get_page(heap->start_address + i, 1, g_kernel_directory),
                    heap->supervisor,
                    !heap->readonly);
    }
    heap->end_address = heap->start_address + new_size;
}

/**
 * Allocate a block of 'size' bytes from the heap, optionally page-aligned.
 * Manages splitting and coalescing of free holes.
 */
void *alloc(uint32_t size, uint8_t page_align, heap_t *heap)
{
    uint32_t total = size + sizeof(header_t) + sizeof(footer_t);
    int32_t  idx = find_smallest_hole(total, page_align, heap);
    if(idx == -1) {
        uint32_t old_len = heap->end_address - heap->start_address;
        expand(old_len + total, heap);
        return alloc(size, page_align, heap);
    }
    header_t *orig = (header_t *)ordered_array_place(idx, &heap->index);
    uint32_t  orig_pos = (uint32_t)orig;
    uint32_t  orig_size = orig->size;

    ordered_array_remove(idx, &heap->index);
    uint32_t offset
        = page_align ? (PAGE_SIZE - ((orig_pos + sizeof(header_t)) % PAGE_SIZE))
                           % PAGE_SIZE
                     : 0;
    if(offset) {
        orig_pos += offset;
        orig_size -= offset;
    }

    uint32_t split = orig_size - total;
    if(split > sizeof(header_t) + sizeof(footer_t)) {
        header_t *hole = (header_t *)(orig_pos + total);
        hole->magic = HEAP_MAGIC;
        hole->size = split;
        hole->is_hole = 1;
        footer_t *hole_footer
            = (footer_t *)((uint32_t)hole + split - sizeof(footer_t));
        hole_footer->magic = HEAP_MAGIC;
        hole_footer->header = hole;
        ordered_array_insert((void *)hole, &heap->index);
    } else {
        total = orig_size;
    }

    header_t *block = (header_t *)orig_pos;
    block->magic = HEAP_MAGIC;
    block->size = total;
    block->is_hole = 0;
    footer_t *block_footer = (footer_t *)(orig_pos + sizeof(header_t) + size);
    block_footer->magic = HEAP_MAGIC;
    block_footer->header = block;
    return (void *)(orig_pos + sizeof(header_t));
}
