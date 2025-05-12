# include "printk.h"

static void putnbr_hex_upper(unsigned int nbr, int *ret)
{
    int     local_ret = 0;
    char    *base = "0123456789ABCDEF";

    if (nbr > 16)
        putnbr_hex_upper(nbr / 16, ret);
    terminal_putchar(base[nbr%16]);
}

static void putnbr_hex(unsigned int nbr, int *ret)
{
    int     local_ret = 0;
    char    *base = "0123456789abcdef";

    if (nbr > 16)
        putnbr_hex_upper(nbr / 16, ret);
    terminal_putchar(base[nbr%16]);
}

static void print_hex(char c, unsigned int nbr, int *ret)
{
    if (c == 'X') {
        putnbr_hex_upper(nbr, ret);
    } else {
        putnbr_hex(nbr, ret);
    }
}


static void print_dec(int nbr, int *ret)
{
    int             local_ret = 0;
    char            to_print;
    unsigned int    nb = 0;

    if (nbr < 0) {
        local_ret = write(1, "-", 1);
        nb = (int)(nbr * -1);
    } else {
        nb = nbr;
    }

    if (nb > 10) 
        print_dec(nb / 10, ret);
    to_print = (nb % 10) - '0';
    write(1, &to_print, 1);
}

static void putnbr_pointer(unsigned long long nbr, int *ret)
{
    int     local_ret = 0;
    char    *base = "0123456789abcdef";

    if (nbr > 16)
        putnbr_pointer(nbr / 16, ret);
    write(1, &base[nbr % 16], 1);
}

static void print_pointer(void *addr, int *ret)
{
    int local_ret = 0;
    unsigned long long addr_nbr = (unsigned long long)addr;
    local_ret = write(1, "0x", 2);
    putnbr_pointer(addr_nbr, ret);
}

// definition of the function call to printk function is in io.c
void    decode_fmt_string(char c, va_list *ap, int *ret)
{
    const char *to_print;

    switch (c) {
    case 'x':
    case 'X':
        print_hex(c, (unsigned int)va_arg(*ap, unsigned int), ret);
        break;
    case 'd':
    case 'i':
        print_dec((int)va_arg(*ap, int), ret);
        break;
    case 's':
        to_print = (const char *)va_arg(*ap, const char *);
        write(1, to_print, strlen(to_print));
        break;
    case 'p':
        print_pointer((void *)va_arg(*ap, void *), ret);
        break;
    default:
        write(1, "(format not handle)", 18);
    }
}
