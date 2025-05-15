#ifndef KHEAP_H
#define KHEAP_H

#include "orderedArray.h"
#include <stdint.h>

#define KHEAP_START         0xC000000
#define KHEAP_INITIAL_SIZE  0x1000000

#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC      0xDEADBEEF
#define HEAP_MIN_SIZE   0x70000

extern uint32_t end;


typedef struct header_s
{
    uint32_t    magic;
    uint8_t     is_hole;
    uint32_t    size;
} header_t;

typedef struct footer_s
{
    uint32_t    magic;
    header_t    *header;
}   footer_t;

typedef struct heap_s
{
    ordered_array_t index;
    uint32_t        start_address;
    uint32_t        end_address;
    uint32_t        max_address;
    uint8_t         supervisor;
    uint8_t         readonly;
} heap_t;

uint32_t kmalloc_aligned(uint32_t size);

uint32_t kmalloc_p(uint32_t size, uint32_t *physical_address);

uint32_t kmalloc_alignedp(uint32_t size, uint32_t *physical_address);

uint32_t kmalloc(uint32_t size);

heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly);

void *alloc(uint32_t size, uint8_t page_align, heap_t *heap);

#endif