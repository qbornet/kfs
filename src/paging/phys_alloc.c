#include <paging/phys_alloc.h>
page_frame_t g_end_frame_map;

/*
 * Find the first physical address FREE available, return the address.
 *
 * @ret page_frame_t (typedef uint32_t *)
 * */
page_frame_t phys_alloc(void)
{
    uint32_t     i = 0;
    page_frame_t frame_map = &g_kernel_end;
    while ((frame_map + i) != g_end_frame_map && frame_map[i] != FREE) {
        i++;
    }
    frame_map[i] = USED;
    return (page_frame_t)(g_kernel_end + (i * FRAME_SIZE));
}

/*
 * Return the first preallocated available page_frame
 * allocate preallocated 20 pages if none is available this will allow us,
 * to have faster page availables without the need to check everytimes if pages,
 * are available.
 *
 * @ret page_frame_t (typedef uint32_t *)
 * */
page_frame_t kalloc_frame()
{
    static uint8_t      allocate = 1;
    static uint8_t      pframe = 0;
    static page_frame_t pre_frames[20];
    page_frame_t        ret;

    if (pframe == 20) allocate = 1;

    if (allocate == 1) {
        for (int i = 0; i < 20; i++) {
            pre_frames[i] = phys_alloc();
        }
        pframe = 0;
        allocate = 0;
    }
    ret = pre_frames[pframe++];
    return ret;
}

/*
 * Free physical page frame given this doesn't zero set the page frame only set
 * the tracker to FREE, only the first 4 byte are written.
 *
 * @frame: Page frame that you want to free
 * */
void kfree_frame(page_frame_t frame)
{
    // give us the offset of the "frame_map" (g_kernel_end);
    page_frame_t frame_map = &g_kernel_end;
    frame = (page_frame_t)(frame - frame_map);
    if (frame == 0) {
        uint32_t index = (uint32_t)frame;
        frame_map[index] = FREE;
    } else {
        uint32_t index = ((uint32_t)frame / FRAME_SIZE);
        frame_map[index] = FREE;
    }
}

/*
 * Initialize the page frame end for the max memory available in the system.
 *
 * @max_mem: Max memory in byte, present in the system (physical memory).
 * */
void init_frame_page(uint32_t max_mem)
{
    // give the max size of the physical address available. (max memory)
    g_end_frame_map = &g_kernel_end + max_mem;
}
