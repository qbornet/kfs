#ifndef MEM_H
# define MEM_H
# include <stdint.h>
# include <stddef.h>

void    *memcpy(void *dst, const void *src, size_t size);
void    *memset(void *ptr, uint8_t byte, size_t size);
#endif
