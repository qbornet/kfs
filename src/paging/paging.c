#include "paging.h"

static bool is_physical_addr_aligned(void *physical_addr)
{
    uintptr_t to_check = (uintptr_t)physical_addr;

    // check if last 12 bits are 0 return true if so.
    if (!((to_check >> 12) & 0xfff)) {
        return true;
    }
    return false;
}
