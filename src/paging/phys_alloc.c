#include <paging/phys_alloc.h>

/*
 * Find the first physical address FREE available, return the address.
 *
 * @ret page_frame_t (typedef uint32_t *)
 * */
page_frame_t phys_alloc(void)
{
    page_frame_t frame_map = &g_kernel_end;
    uint32_t     i = 0;
    while ((frame_map + i) != g_end_frame_map && frame_map[i] != FREE) {
        i += FRAME_SIZE;
    }
    frame_map[i] = USED;
    return (page_frame_t)(g_kernel_end + i);
}

/*
 * Free physical page frame given this doesn't zero set the page frame only set
 * the tracker to FREE, only the first 4 byte are written.
 *
 * @phys: Physical address should point to the begining of the physical address
 * */
void phys_free(page_frame_t phys)
{
    if (*phys != FREE) *phys = FREE;
}

/*
 * Initialize the page frame end for the max memory available in the system.
 *
 * @max_mem: Max memory in byte, present in the system (physical memory).
 * */
void init_frame_page(uint32_t max_mem)
{
    // give the max size of the physical address available. (max memory)
    g_end_frame_map = (page_frame_t)&g_kernel_end + max_mem;
}
