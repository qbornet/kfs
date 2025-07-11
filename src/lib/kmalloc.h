#ifndef KMALLOC_H
#define KMALLOC_H
#include "./shared/defs.h"
#include "io.h"
#include "string.h"
#include <stddef.h>
#include <stdint.h>

// 16 KiB For kernel heap
#define HEAP_SIZE         0x4000

// Offset so that we dont write our global in memory.
#define HEAP_OFFSET_START 0x100;

// 4 KiB Page size
#define PAGE_SIZE         0x1000

// Available free memory pointer (meaning that it's the last known available,
// free memory)
extern uintptr_t g_free_mem_ptr;

// init malloc pointer for kernel heap (physical memory).
void             init_malloc_ptr(void);

#endif
