#ifndef IO_H
# define IO_H
# include "printk.h"
# include "mem.h"

// Print a area of memory start at the addr end at addr + len.
void    print_memory(void *addr, size_t len);

// limited equivalent of printf but for the kernel !
int     printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#endif
