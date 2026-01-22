#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H
#define FRAME_SIZE           4096
#define BITMAP_SIZE(max_mem) (max_mem / FRAME_SIZE / 8)
#define FREE                 0
#define USED                 1
#include <lib/io.h>
#include <stddef.h>
#include <stdint.h>
typedef uint32_t *page_frame_t;

/*
 * Return the first preallocated available page_frame
 * allocate preallocated 20 pages if none is available this will allow us,
 * to have faster page availables without the need to check everytimes if pages,
 * are available.
 *
 * @ret page_frame_t (typedef uint32_t *)
 * */
page_frame_t      kalloc_frame(void);

/*
 * Free physical page frame given this doesn't zero set the page frame only set
 * the tracker to FREE, only the first 4 byte are written.
 *
 * @frame: Page frame that you want to free
 * */
void              kfree_frame(page_frame_t);

void              init_frame_page(uint32_t base_addrs, uint32_t size);
#endif
