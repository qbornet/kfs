#include "cpl.h"

static __always_inline uint32_t get_cpl(void)
{
    uint32_t cpl;

    asm volatile(".intel_syntax noprefix\n\t"
                 "mov ax, cs\n\t"
                 "and ax, 0x03\n\t"
                 "mov %0, eax\n\t"
                 ".att_syntax prefix\n\t"
                 : "=r"(cpl)::"eax");
    return cpl;
}

uint8_t get_current_level_privilege(void)
{
    return get_cpl();
}

bool is_ring3(void)
{
    return get_cpl() == 3 ? true : false;
}

bool is_ring2(void)
{
    return get_cpl() == 2 ? true : false;
}

bool is_ring1(void)
{
    return get_cpl() == 1 ? true : false;
}

bool is_ring0(void)
{
    return get_cpl() == 0 ? true : false;
}
