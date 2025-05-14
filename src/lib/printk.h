#ifndef PRINTK_H
# define PRINTK_H
# include "string.h"
# include "mem.h"
# include "ctype.h"
# include "../vga/vga.h"
# include <stdarg.h>
# include <stdint.h>

typedef struct param_indentifier {
    bool        present;
    char        leading;
    uint32_t    max;
    uint32_t    count;
    uint32_t    writen;
} __attribute__((packed)) prm_indentifier_t;

void    decode_fmt_string_len(const char *str, va_list *ap, prm_indentifier_t *prm);
void    decode_fmt_string(const char *str, prm_indentifier_t *prm, int *index, va_list *ap, int *ret);
#endif
