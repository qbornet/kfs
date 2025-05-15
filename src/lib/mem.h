#ifndef MEM_H
# define MEM_H
# include <stdint.h>
# include <stddef.h>

// copy size byte to dst pointer from src pointer.
void    *memcpy(void *dst, const void *src, size_t size);

// set byte to ptr until size is reached.
void    *memset(void *ptr, uint8_t byte, size_t size);
#endif
