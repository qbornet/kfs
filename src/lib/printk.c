#include "printk.h"

static inline void putnbr_hex_upper(unsigned int nbr, int *ret)
{
    char *base = "0123456789ABCDEF";

    if(nbr >= 16) putnbr_hex_upper(nbr / 16, ret);
    terminal_putchar(base[nbr % 16]);
}

static inline void putnbr_hex(unsigned int nbr, int *ret)
{
    char *base = "0123456789abcdef";

    if(nbr >= 16) putnbr_hex_upper(nbr / 16, ret);
    terminal_putchar(base[nbr % 16]);
}

static inline void print_hex(char c, unsigned int nbr, int *ret)
{
    if(c == 'X') {
        putnbr_hex_upper(nbr, ret);
    } else {
        putnbr_hex(nbr, ret);
    }
}

static inline void print_dec(int nbr, int *ret)
{
    unsigned int nb = 0;

    if(nbr < 0) {
        terminal_putchar('-');
        nb = (int)(nbr * -1);
        *ret += 1;
    } else {
        nb = nbr;
    }

    if(nb >= 10) print_dec(nb / 10, ret);
    terminal_putchar(nb % 10 + '0');
}

static inline void putnbr_pointer(unsigned long long nbr, int *ret)
{
    char *base = "0123456789abcdef";

    if(nbr >= 16) putnbr_pointer(nbr / 16, ret);
    terminal_putchar(base[nbr % 16]);
    *ret += 1;
}

static inline void print_pointer(void *addr, int *ret)
{
    uintptr_t addr_nbr = (uintptr_t)addr;
    terminal_writestring("0x", 0);
    putnbr_pointer(addr_nbr, ret);
}

// definition of the function call to printk function is in io.c
void decode_fmt_string(
    const char *str, prm_indentifier_t *prm, int *index, va_list *ap, int *ret)
{
    const char *to_print;
    const char  c = *str;
    va_list     ap_cpy;

    va_copy(ap_cpy, *ap);
    switch(c) {
        case 'x':
        case 'X':
            print_hex(c, (unsigned int)va_arg(*ap, unsigned int), ret);
            *index += 2;
            break;
        case 'd':
        case 'i':
            print_dec((int)va_arg(*ap, int), ret);
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
            if(str[1] && isdigit(str[1])) {
                prm->present = true;
                prm->leading = str[0];
                prm->max = (uint32_t)(str[1] - '0');
            }

            if(prm->present == true) {
                *index += 2;
                decode_fmt_string_len(&str[2], &ap_cpy, prm);
                va_end(ap_cpy);
                if(prm->writen < prm->max) {
                    for(uint32_t i = 0; i < (prm->max - prm->writen); i++)
                        terminal_putchar(prm->leading);
                }
                decode_fmt_string(&str[2], prm, index, ap, ret);
            } else {
                terminal_writestring("(format not handle)", 0);
            }
    }
}
