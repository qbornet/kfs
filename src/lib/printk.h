#ifndef PRINTK_H
# define PRINTK_H
# include "string.h"
# include "../vga/vga.h"
# include <stdarg.h>

void    decode_fmt_string(char c, va_list *ap, int *ret);
#endif
