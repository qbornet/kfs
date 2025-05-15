#include "printk.h"

static inline void putnbr_hex_len(unsigned int nbr, prm_indentifier_t *prm)
{
    if(nbr >= 16) putnbr_hex_len(nbr / 16, prm);
    prm->writen += 1;
}

static inline void putnbr_len(int nbr, prm_indentifier_t *prm)
{
    if(nbr >= 10) putnbr_hex_len(nbr / 10, prm);
    prm->writen += 1;
}

void decode_fmt_string_len(const char *str, va_list *ap, prm_indentifier_t *prm)
{
    const char c = *str;

    switch(c) {
        case 'x':
        case 'X':
            putnbr_hex_len((unsigned int)va_arg(*ap, unsigned int), prm);
            break;
        case 'd':
        case 'i': putnbr_len((int)va_arg(*ap, int), prm); break;
        default: break;
    }
}
