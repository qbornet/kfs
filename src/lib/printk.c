#include "printk.h"

static __always_inline void putnbr_hex_upper(unsigned long nbr, int *ret)
{
    uint32_t written;

    written = 0;
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t nibble = (nbr >> shift) & 0xF;
        if (!nibble) continue;
        written++;
        terminal_putchar(nibble < 10 ? '0' + nibble : 'A' + nibble - 10);
        *ret += 1;
    }
    if (!written) terminal_putchar('0');
}

static __always_inline void putnbr_hex(unsigned long nbr, int *ret)
{
    uint32_t written;

    written = 0;
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t nibble = (nbr >> shift) & 0xF;
        if (!nibble) continue;
        written++;
        terminal_putchar(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
        *ret += 1;
    }
    if (!written) terminal_putchar('0');
}

static __always_inline void print_hex(char c, unsigned long nbr, int *ret)
{
    if (c == 'X') {
        putnbr_hex_upper(nbr, ret);
    } else {
        putnbr_hex(nbr, ret);
    }
}

static __always_inline void print_udec(unsigned long nbr, int *ret)
{
    uint32_t written;

    written = 0;
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t nibble = (nbr >> shift) & 0xF;
        if (!nibble) continue;
        written++;
        terminal_putchar('0' + nibble);
        *ret += 1;
    }
    if (!written) terminal_putchar('0');
}

static __always_inline void print_dec(long nbr, int *ret)
{
    unsigned int nb = 0;

    if (nbr < 0) {
        terminal_putchar('-');
        nb = (int)(nbr * -1);
        *ret += 1;
    } else {
        nb = nbr;
    }
    print_udec(nb, ret);
}

static __always_inline void putnbr_pointer(unsigned long long nbr, int *ret)
{
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t nibble = (nbr >> shift) & 0xF;
        terminal_putchar(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
        *ret += 1;
    }
}

static __always_inline void print_pointer(void *addr, int *ret)
{
    uintptr_t addr_nbr = (uintptr_t)addr;
    terminal_writestring("0x", 0);
    putnbr_pointer(addr_nbr, ret);
}

// definition of the function call to printk function is in io.c
void decode_fmt_string(
    const char *str, printk_opt_t *opt, int *index, va_list *ap, int *ret)
{
    const char             *to_print;
    const char              c = *str;
    va_list                 ap_cpy;
    prm_pad_char_modifier_t char_modifier = opt->pad_char_modifier;
    prm_len_modifier_t      len_modifier = opt->len_modifier;

    va_copy(ap_cpy, *ap);
    switch (c) {
        case 'l':
            len_modifier.present = true;
            len_modifier.long_present = true;
            opt->pad_char_modifier = char_modifier;
            opt->len_modifier = len_modifier;
            *index += 1;
            decode_fmt_string(&str[1], opt, index, ap, ret);
            break;
        case 'x':
        case 'X':
            if (len_modifier.present && len_modifier.long_present) {
                print_hex(c, (unsigned long)va_arg(*ap, unsigned long), ret);
            } else {
                print_hex(c, (unsigned int)va_arg(*ap, unsigned int), ret);
            }
            *index += 2;
            break;
        case 'u':
            if (len_modifier.present && len_modifier.long_present) {
                print_udec((unsigned long)va_arg(*ap, unsigned long), ret);
            } else {
                print_udec((unsigned int)va_arg(*ap, unsigned long), ret);
            }
            *index += 2;
            break;
        case 'd':
        case 'i':
            if (len_modifier.present && len_modifier.long_present) {
                print_dec((long)va_arg(*ap, long), ret);
            } else {
                print_dec((int)va_arg(*ap, long), ret);
            }
            *index += 2;
            break;
        case 's':
            to_print = (const char *)va_arg(*ap, const char *);
            terminal_writestring(to_print, 0);
            *index += 2;
            break;
        case 'p':
            print_pointer((void *)va_arg(*ap, void *), ret);
            *index += 2;
            break;
        default:
            if (str[1] && isdigit(str[1])) {
                char_modifier.present = true;
                char_modifier.leading = str[0];
                char_modifier.max = (uint32_t)(str[1] - '0');
            }

            if (char_modifier.present == true) {
                *index += 2;
                decode_fmt_string_len(&str[2], &ap_cpy, &char_modifier);
                va_end(ap_cpy);
                if (char_modifier.writen < char_modifier.max) {
                    for (uint32_t i = 0;
                         i < (char_modifier.max - char_modifier.writen);
                         i++)
                        terminal_putchar(char_modifier.leading);
                }
                opt->len_modifier = len_modifier;
                opt->pad_char_modifier = char_modifier;
                decode_fmt_string(&str[2], opt, index, ap, ret);
            } else {
                terminal_writestring("(format not handle)", 0);
            }
    }
}
