#ifndef PAGE_FAULT_H
#define PAGE_FAULT_H
#include <lib/io.h>
#include <stdint.h>

typedef struct s_register {
    uint32_t edi, esi, ebp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} register_t;

#endif
