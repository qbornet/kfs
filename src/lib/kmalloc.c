#include <lib/kmalloc.h>
#include <paging/paging.h>
#include <paging/pfn.h>
#include <paging/phys_alloc.h>

void *kmalloc(uint32_t size, uint8_t flags)
{
    if (size == 0) return NULL;

    void    *ret = NULL;
    uint8_t  first_page = 0;
    uint32_t virtual_base = 0;
    if (!flags || flags & SP_KERNEL) {
        virtual_base = g_kernel_start_virt_mem;
    } else if (flags & SP_USER) {
        virtual_base = g_user_start_virt_mem;
    }
    void *frame = pmalloc(size);
    if (!frame) {
        printk("pmalloc failed\n");
        return NULL;
    }

    pfn_t *page = get_page_info((uint32_t)frame);
    if (!page) {
        printk("get_page_info failed\n");
        return NULL;
    }

    for (uint32_t i = 0; i < page->alloc_size; i++) {
        page_frame_t tmp = (page_frame_t)((uint32_t)frame + (i * FRAME_SIZE));
        virtual_base += (i * FRAME_SIZE);
        if (!first_page) {
            ret = get_memory_page(tmp, virtual_base, flags);
            first_page = 1;
            continue;
        }
        get_memory_page(tmp, virtual_base, flags);
    }
    if (!flags || flags & SP_KERNEL) {
        g_kernel_start_virt_mem += (page->alloc_size * FRAME_SIZE);
    } else if (flags & SP_USER) {
        g_user_start_virt_mem += (page->alloc_size * FRAME_SIZE);
    }
    return ret;
}

void kfree(void *vaddr)
{
    if (!vaddr) return;

    pfn_t *page
        = get_page_info(V2P(vaddr)); // Need to add V2P for user space memory ?
    if (!page) return;

    for (uint32_t i = 0; i < page->alloc_size; i++) {
        destroy_memory_page(vaddr + (i * FRAME_SIZE));
    }
}

uint32_t ksize(void *vaddr)
{
    if (!vaddr) return -1;
    uint32_t phys = V2P(vaddr);

    if (!phys) return -1;
    pfn_t *page = get_page_info(phys);

    if (!page) return -1;
    return page->alloc_size;
}
