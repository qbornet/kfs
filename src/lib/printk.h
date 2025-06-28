#ifndef PRINTK_H
#define PRINTK_H
#include "../lib/shared/defs.h"
#include "../vga/vga.h"
#include "ctype.h"
#include "mem.h"
#include "string.h"
#include <stdarg.h>
#include <stdint.h>

// struct for optional param indentifier for printk.
typedef struct param_padding_character_modifier {
    bool     present;
    char     leading;
    uint32_t max;
    uint32_t writen;
} __attribute__((packed)) prm_pad_char_modifier_t;

typedef struct param_length_modifier {
    bool present;
    bool long_present;
} __attribute__((packed)) prm_len_modifier_t;

typedef struct printk_options {
    prm_pad_char_modifier_t pad_char_modifier;
    prm_len_modifier_t      len_modifier;
} __attribute__((packed)) printk_opt_t;

// get the len of the write of %Xxio
void                      decode_fmt_string_len(const char              *str,
                                                va_list                 *ap,
                                                prm_pad_char_modifier_t *prm);

// start decode format string to print variadic parameter.
void                      decode_fmt_string(
                         const char *str, printk_opt_t *prm, int *index, va_list *ap, int *ret);
#endif
