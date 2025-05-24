#ifndef KMALLOC_H
#define KMALLOC_H

#include "../memory/mmu.h"
#include <stddef.h>
#include <stdint.h>

/**
 * kmalloc - Allocate memory from the kernel heap.
 * @size: Number of bytes to allocate.
 * Returns a pointer aligned to machine word or NULL on failure.
 */
void *kmalloc(size_t size);

/**
 * kmalloc_aligned - Allocate page-aligned memory.
 * @size: Number of bytes to allocate.
 * Returns a pointer aligned to PAGE_SIZE boundary.
 */
void *kmalloc_aligned(size_t size);

#endif