#ifndef GDT_H
#define GDT_H

// set bit system on
#define BIT_SYSTEM_ON              1
#define BIT_SYSTEM_OFF             0

// set in byte access, exec bit
#define TYPE_EXEC_ON               1
#define TYPE_EXEC_OFF              0

// set in byte access, rw bit
#define TYPE_RW_ON                 1
#define TYPE_RW_OFF                0

// set in byte access, dc bit
#define TYPE_DC_ON                 1
#define TYPE_DC_OFF                0

// set in byte access flag, access bit
#define TYPE_A_ON                  1
#define TYPE_A_OFF                 0

// set in flags byte, granularity bit (23 bit)
#define FLAGS_GRANULARITY_ON       1
#define FLAGS_GRANULARITY_OFF      0

// set in flags byte, mode bit (22 bit)
#define FLAGS_MODE_ON              1
#define FLAGS_MODE_OFF             0

// set in flags byte, avl bit (20 bit)
#define FLAGS_AVL_64_ON            1
#define FLAGS_AVL_64_OFF           0

// set in access byte, s bit (1 code or data segment)
#define ACCESS_DESCRIPTOR_TYPE_ON  1

// set in access byte, s bit (0 system segment like TSS)
#define ACCESS_DESCRIPTOR_TYPE_OFF 0

// set in access byte, privilege level of segment (DPL)
#define ACCESS_DPL_RING_0          0
#define ACCESS_DPL_RING_1          1
#define ACCESS_DPL_RING_2          2
#define ACCESS_DPL_RING_3          3

#include "lib/io.h"
#include "lib/mem.h"
#include "lib/shared.h"
#include "tss.h"
#include <stdbool.h>
#include <stdint.h>

// segment_descriptor based on the intel IA-32
struct segment_descriptor {
    uint32_t start;
    uint32_t end;
} __attribute__((packed));

typedef struct segment_descriptor  sd_t;
typedef struct segment_descriptor *sd_ptr;

// gdt struct to hold the size + address 6 byte for pmode, needed for
// instruction lgdt.
struct gdt {
    uint16_t size;
    uint32_t address;
} __attribute__((packed));

void init_gdt(void);
void jump_usermode(void);
#endif
