#include "printk.h"

static __always_inline void putnbr_hex_len(unsigned int             nbr,
                                           prm_pad_char_modifier_t *prm)
{
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t nibble = (nbr >> shift) & 0xF;
        if (!nibble) continue;
        prm->writen += 1;
    }
    if (!prm->writen) prm->writen += 1;
}

static __always_inline void putnbr_len(int nbr, prm_pad_char_modifier_t *prm)
{
    for (int shift = 28; shift >= 0; shift -= 4) {
        uint8_t nibble = (nbr >> shift) & 0xF;
        if (!nibble) continue;
        prm->writen += 1;
    }
    if (!prm->writen) prm->writen += 1;
}

void decode_fmt_string_len(const char              *str,
                           va_list                 *ap,
                           prm_pad_char_modifier_t *prm)
{
    const char c = *str;

    switch (c) {
        case 'x':
        case 'X':
            putnbr_hex_len((unsigned int)va_arg(*ap, unsigned int), prm);
            break;
        case 'd':
        case 'i': putnbr_len((int)va_arg(*ap, int), prm); break;
        default: break;
    }
}
