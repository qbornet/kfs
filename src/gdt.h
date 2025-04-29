#ifndef GDT_H
# define GDT_H

// set bit system on
# define BIT_SYSTEM_ON 1
# define BIT_SYSTEM_OFF 0

// set in byte access, exec bit
# define TYPE_EXEC_ON 1
# define TYPE_EXEC_OFF 0

// set in byte access, rw bit
# define TYPE_RW_ON 1
# define TYPE_RW_OFF 0

// set in byte access, dc bit
# define TYPE_DC_ON 1
# define TYPE_DC_OFF 0

// set in byte access flag, access bit 
# define TYPE_A_ON 1
# define TYPE_A_OFF 0

// set in flags byte, granularity bit
# define FLAGS_GRANULARITY_ON 1
# define FLAGS_GRANULARITY_OFF 0

// set in flags byte, mode bit
# define FLAGS_MODE_ON 1
# define FLAGS_MODE_OFF 0

# include <stdint.h>
# include <stdbool.h>
# include "lib/mem.h"

struct segment_descriptor {
    uint32_t    start;
    uint32_t    end;
} __attribute__((packed));

typedef struct segment_descriptor   sd_t;
typedef struct segment_descriptor*  sd_ptr;

struct gdt {
    uint16_t    size;
    uint32_t    address;
} __attribute__((packed));

void    init_gdt(void);
#endif
