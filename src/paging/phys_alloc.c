#include <paging/phys_alloc.h>
static uint8_t  g_phys_bitmap[4096];
static uint32_t g_start_phys_mem;
static uint32_t g_pre_frames[20];
page_frame_t    g_end_frame_map;

/*
 * Find the first physical address FREE available, return the address.
 *
 * @ret page_frame_t (typedef uint32_t *)
 * */
page_frame_t    phys_alloc(void)
{
    uint32_t i = 0;
    while (i < FRAME_SIZE && g_phys_bitmap[i] != FREE) {
        i++;
    }
    g_phys_bitmap[i] = USED;
    page_frame_t ret = (page_frame_t)(g_start_phys_mem + (i * FRAME_SIZE));
    printk("ret: %p\n", ret);
    return ret;
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
    static uint8_t allocate = 1;
    static uint8_t pframe = 0;

    if (pframe == 20) allocate = 1;

    if (allocate == 1) {
        printk("allocate is on\n");
        for (int i = 0; i < 20; i++) {
            printk("[%d]\n", i);
            g_pre_frames[i] = (uint32_t)phys_alloc();
            printk("pre_frames[%d]: %x\n", i, g_pre_frames[i]);
        }
        pframe = 0;
        allocate = 0;
    }
    return (page_frame_t)g_pre_frames[pframe++];
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
    page_frame_t frame_map = (page_frame_t)g_start_phys_mem;
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
void init_frame_page(uint32_t base_addrs, uint32_t size)
{
    g_start_phys_mem = base_addrs;
    g_end_frame_map = (page_frame_t)(base_addrs + size);
    printk("g_start_phys_mem: 0x%x, g_end_frame_map: %p, cast: %p\n",
           g_start_phys_mem,
           g_end_frame_map,
           (page_frame_t)g_start_phys_mem);
}
