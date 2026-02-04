#ifndef KMALLOC_H
#define KMALLOC_H
#include <paging/paging.h>
#include <paging/pfn.h>
#include <paging/phys_alloc.h>
#include <stdint.h>

#define SP_KERNEL 0x01
#define SP_USER   0x02

uint32_t ksize(void *vaddr);
void     kfree(void *vaddr);
void    *kmalloc(uint32_t size, uint8_t flags);
#endif
