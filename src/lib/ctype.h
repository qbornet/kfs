#ifndef CTYPE_H
#define CTYPE_H
#include <lib/shared/defs.h>

#undef isdigit
// return positive number if c is a digit.
__always_inline int isdigit(int c)
{
    return (c >= 48 && c <= 57);
}

#endif
