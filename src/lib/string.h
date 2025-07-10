#ifndef STRING_H
#define STRING_H
#include <stddef.h>

#undef strlen

// function calculates the length of the string pointed to by str, excluding the
// terminating null byte.
size_t strlen(const char *str);

/* Copy src to dst programer should be carefull when using strcpy,
 * you have the responsability of the size of 'dst being len(src) + 1.
 *
 * @dst: Destination buffer len should be len(src) + 1
 * @src: Source buffer
 */
char  *strcpy(char *dst, const char *src);

#endif
