#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H
#define MAX_MEM     0x100000 * 128 // 128MiB
#define FRAME_SIZE  4096
#define BITMAP_SIZE (MAX_MEM / FRAME_SIZE / 8)
#endif
