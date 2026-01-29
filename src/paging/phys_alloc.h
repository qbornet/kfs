#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H
#define MAX_PAGES   (1024 * 1024)
#define FRAME_SIZE  4096
#define BITMAP_SIZE 32768
#define FREE        0x0
#define USED        0xFFFFFFFF
#include <lib/io.h>
#include <stddef.h>
#include <stdint.h>
typedef uint32_t *page_frame_t;

uint32_t          psize(void *phys_addr);
void             *pmalloc(uint32_t size);
void              pfree(void *phys_addr);
void              init_frame_page(uint32_t base_addrs, uint32_t size);
extern uint32_t   g_total_pages;
extern uint32_t   g_start_phys_mem;
#endif
