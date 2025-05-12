#include "io.h"

// printk is a limited printf function for kernel only.
int printk(const char *fmt, ...)
{
    int i = 0;
    int ret = 0;
    int local_ret = 0;
    va_list ap;
    va_start(ap, fmt);
    while (fmt[i]) {
        if (fmt[i] == '%') {
            decode_fmt_string(fmt[i++], &ap, &ret);
        } 
        i++;
    }
    va_end(ap);
    return ret;
}
