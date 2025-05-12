#ifndef IO_H
# define IO_H
# include "printk.h"

int    printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#endif
