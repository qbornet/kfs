#ifndef CPL_H
#define CPL_H
#include "shared/defs.h"
#include <stdbool.h>
#include <stdint.h>

bool    is_ring3(void);
bool    is_ring2(void);
bool    is_ring1(void);
bool    is_ring0(void);
uint8_t get_current_level_privilege(void);
#endif
