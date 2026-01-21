#ifndef KERNEL_EARLY_H
#define KERNEL_EARLY_H
#include <gdt.h>
#include <lib/io.h>
#include <lib/mem.h>
#include <multiboot2.h>
#include <multiboot2_info.h>
#include <stdint.h>
#define KERNEL_OFFSET 0xBFF00000
#define V2P(a)        ((uintptr_t)(a) & ~KERNEL_OFFSET)
#define P2V(a)        ((uintptr_t)(a) | KERNEL_OFFSET)

extern void jump_to_higher_half(void);
#endif
