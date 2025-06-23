#include "io.h"

void print_memory(void *addr, size_t len)
{
    unsigned int groups;
    unsigned int counter;
    void        *current_addr;

    counter = 0;
    add_terminal_row(1);
    set_terminal_column(0);
    for (uint32_t i = 0; i < len; i++) {
        if (i % 16 == 0) {
            current_addr = (void *)(addr + i);
            if (counter++ == 0)
                printk("%p: ", current_addr);
            else
                printk("\n%p: ", current_addr);
        }
        groups = *(uint8_t *)(addr + i);
        if (i % 2 == 0) printk(" ");
        printk("%02X", groups);
    }
}

int printk(const char *fmt, ...)
{
    int          i = 0;
    int          ret = 0;
    va_list      ap;
    printk_opt_t options;

    memset(&options, 0, sizeof(printk_opt_t));
    va_start(ap, fmt);
    while (fmt[i]) {
        if (fmt[i] == '%') {
            decode_fmt_string(&fmt[i + 1], &options, &i, &ap, &ret);
        } else {
            terminal_putchar(fmt[i]);
            ret++;
            i++;
        }
    }
    va_end(ap);
    return ret;
}
