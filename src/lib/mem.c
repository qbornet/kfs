#include "mem.h"

void    *memset(void *ptr, uint8_t byte, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        *(uint8_t *)(ptr + i) = byte;
    }
    return ptr;
}

void    *memcpy(void *dst, const void *src, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        *(uint8_t *)(dst + i) = *(uint8_t *)src;
    }
    return dst;
}
