#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H
#define FRAME_SIZE           4096
#define BITMAP_SIZE(max_mem) (max_mem / FRAME_SIZE / 8)
#define FREE                 0x00000000
#define USED                 0x32323232
#include <stddef.h>
#include <stdint.h>
typedef uint32_t *page_frame_t;

/*
 * Find the first physical address FREE available, return the address.
 *
 * @ret page_frame_t (typedef uint32_t *)
 * */
page_frame_t      phys_alloc(void);

/*
 * Initialize the page frame end for the max memory available in the system.
 *
 * @max_mem: Max memory in byte, present in the system (physical memory).
 * */
void              init_frame_page(uint32_t max_mem);

extern uint32_t   g_kernel_end;
page_frame_t      g_end_frame_map;
#endif
