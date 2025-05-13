#include "io.h"

void    print_memory(void  *addr, size_t len)
{
    unsigned int    groups;
    void            *current_addr;

    for (uint32_t i = 0; i < len; i++) {
        if (i % 16 == 0) {
            current_addr = (void *)(addr + i);
            printk("%p: ", current_addr);
        }
        groups = *(unsigned int*)(addr + i * 4);
        printk("%X ", groups);
    }
}

// printk is a limited printf function for kernel only.
int printk(const char *fmt, ...)
{
    int i = 0;
    int ret = 0;
    va_list ap;
    va_start(ap, fmt);
    while (fmt[i]) {
        if (fmt[i] == '%') {
            decode_fmt_string(fmt[i+1], &ap, &ret);
            i += 2;
        } else {
            terminal_putchar(fmt[i]);
            ret++;
            i++;
        }
    }
    va_end(ap);
    return ret;
}
