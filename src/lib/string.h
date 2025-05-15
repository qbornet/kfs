#ifndef STRING_H
# define STRING_H
# include <stddef.h>

# undef strlen

// function calculates the length of the string pointed to by str, excluding the terminating null byte.
size_t  strlen(const char *str);

#endif
