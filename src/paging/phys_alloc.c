#include <paging/pfn.h>
#include <paging/phys_alloc.h>
static uint32_t g_phys_bitmap[BITMAP_SIZE];
static uint32_t g_start_phys_mem;
static uint32_t g_pre_frames[20];
uint32_t        g_total_pages;
page_frame_t    g_end_frame_map;

// Set bitmap a map is used
static void     bitmap_set(uint32_t bit)
{
    g_phys_bitmap[bit / 32] |= (1 << (bit % 32));
}

// Unset bitmap a map is free
static void bitmap_unset(uint32_t bit)
{
    g_phys_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

// Test bitmap a map is used or free; (0) free (1) used
static int bitmap_test(uint32_t bit)
{
    return g_phys_bitmap[bit / 32] & (1 << (bit % 32));
}

static page_frame_t phys_alloc(void)
{
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        if (g_phys_bitmap[i] == 0xFFFFFFFF) continue;

        for (int j = 0; j < 32; j++) {
            if (!bitmap_test(i * 32 + j)) {
                uint32_t frame_idx = (i * 32) + j;
                if (frame_idx >= g_total_pages) return 0;

                bitmap_set(frame_idx);
                return (page_frame_t)(g_start_phys_mem
                                      + (frame_idx * FRAME_SIZE));
            }
        }
    }
    return 0;
}

static void pfree_frame(page_frame_t frame)
{
    uint32_t addr = (uint32_t)frame;
    if (addr < g_start_phys_mem) return;

    uint32_t frame_idx = (addr - g_start_phys_mem) / FRAME_SIZE;
    if (frame_idx >= g_total_pages) return;
    bitmap_unset(frame_idx);
}

static page_frame_t palloc_frame()
{
    static uint8_t allocate = 1;
    static uint8_t pframe = 0;

    if (pframe == 20) {
        allocate = 1;
        pframe = 0;
    }

    if (allocate == 1) {
        for (int i = 0; i < 20; i++) {
            g_pre_frames[i] = (uint32_t)phys_alloc();
        }
        allocate = 0;
    }
    return (page_frame_t)g_pre_frames[pframe++];
}

/* pmalloc return a physical pointer to a page that need to be free.
 * (this will be a continuous block of memory)
 *
 * TODO: Will need to have a buddy allocator to avoid waste of memory.
 *
 * @size: Size of the variable that you want alloc.
 * */
void *pmalloc(uint32_t size)
{
    uint32_t i = 0;
    void    *ret = palloc_frame();
    while ((++i * FRAME_SIZE) < size) {
        palloc_frame();
    }
    uint32_t index = ((uint32_t)ret - g_start_phys_mem) / FRAME_SIZE;
    pfn_t   *page = &g_mem_map[index];
    page->alloc_size = i;
    page->flags |= PFN_FLAG_USED | PFN_FLAG_KERNEL;
    return ret;
}

/* pfree free a block of pages the address provided should be returned by
 * `pmalloc` (this will be a continuous block of memory)
 *
 * @phys_addr: Physical address return by `pmalloc`
 * */
void pfree(void *phys_addr)
{
    pfn_t *page = get_page_info((uint32_t)phys_addr);
    if (!page) return;

    if (page->flags & PFN_FLAG_USED) {
        void *tmp;
        for (uint32_t i = 0; i < page->alloc_size; i++) {
            tmp = phys_addr + (i * FRAME_SIZE);
            pfree_frame(tmp);
        }
        memset(page, 0, sizeof(pfn_t));
    }
}

/* psize return the size allocated to the phys_addr passed in pages num (4KiB)
 *
 *
 * @phys_addr: Physical address return by `pmalloc`
 * */
uint32_t psize(void *phys_addr)
{
    pfn_t *page = get_page_info((uint32_t)phys_addr);
    if (!page) return -1;
    return page->alloc_size;
}

void init_frame_page(uint32_t base_addrs, uint32_t size)
{
    g_start_phys_mem = base_addrs;

    g_total_pages = size / FRAME_SIZE;

    if (g_total_pages > (BITMAP_SIZE * 32)) {
        g_total_pages = BITMAP_SIZE * 32;
        printk("Warning: Truncated memory we dont handle higher then %dMiB\n",
               MAX_PAGES);
    }

    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        g_phys_bitmap[i] = FREE;
    }
    // The first 32 integer is equivalent to 4MiB which is already allocated.
    for (uint32_t i = 0; i < 32; i++) {
        g_phys_bitmap[i] = USED;
    }
    printk("Physical allocator: Manage %d pages starting at 0x%x\n",
           g_total_pages,
           g_start_phys_mem);
}
