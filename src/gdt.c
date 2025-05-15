#include "gdt.h"
#include <stdint.h>

sd_t                sdes[8];

static inline uint8_t   create_type(uint8_t exec, uint8_t dc, uint8_t rw, uint8_t access)
{
    return access | rw << 1 | dc << 2 | exec << 3;
}

static inline uint8_t   create_flags(uint8_t gran, uint8_t mode, uint8_t avl)
{
    return avl | 0 << 1 | mode << 2 | gran << 3;
}

static inline uint8_t   create_access(uint8_t type, uint8_t system, uint8_t dpl)
{
    return (type & 0xf) | (system & 0x1) << 4 | (dpl & 0x3) << 5 | (1 & 0x1) << 7;
}

static inline void    create_descriptor(uint8_t flags, uint8_t access, uint32_t addr, uint32_t limit, sd_ptr segment_des)
{
    segment_des->start = (limit & 0xffff) | (addr & 0xffff) << 16;
    limit >>= 16;
    addr >>= 16;

    uint8_t low_address = (addr & 0xff);
    addr >>= 8;

    uint8_t high_address = (addr & 0xff);
    addr >>= 8;

    segment_des->end = low_address | (access & 0xff) << 8 | (limit & 0xf) << 16 | (flags & 0xf) << 20 | high_address << 24;
}

// reload only kernel descriptor other descriptor are loaded in gdtr but not used.
static inline void reload_segments(void) {
    __asm__ volatile (
        ".intel_syntax noprefix\n\t"
        "push 0x08\n\t"
        "push offset 1f\n\t"
        "lret\n"
        "1:\n\t"
        "mov ax, 0x10\n\t"
        "mov ds, ax\n\t"
        "mov es, ax\n\t"
        "mov fs, ax\n\t"
        "mov ax, 0x18\n\t"
        "mov ss, ax\n\t"
        "mov ax, 0x20\n\t"
        "mov gs, ax\n\t"
        ".att_syntax prefix\n\t"
        :
        :
        : "ax", "memory"
    );
}

static inline void    load_gdt(void)
{
    struct gdt   _gdt;
    _gdt.size = sizeof(sdes) - 1;
    _gdt.address = (uint32_t)&sdes;

    asm volatile (
            "cld\n\t"
            "mov %0, %%esi\n\t"
            "mov %1, %%edi\n\t"
            "mov %2, %%ecx\n\t"
            "rep movsb\n\t"
            :
            : "r" (&sdes), "r" (0x800), "r" (sizeof(sdes))
            : "esi", "edi", "ecx", "memory"
        );
    asm volatile ("lgdt %0\n\t" :: "m"(_gdt));
}

void    init_gdt(void)
{
    // NULL DESCRIPTOR
    create_descriptor(0, 0, 0, 0, &sdes[0]);

    // KERNEL CODE DESCRIPTOR
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(create_type(TYPE_EXEC_ON, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_0),
        0x00000000,
        0x000FFFFF,
        &sdes[1]
    );
    
    // KERNEL DATA DESCRIPTOR
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(create_type(TYPE_EXEC_OFF, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_0),
        0x00000000,
        0x000FFFFF,
        &sdes[2]
    );

    // KERNEL STACK DESCRIPTOR
    create_descriptor(
            create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
            create_access(create_type(TYPE_EXEC_OFF, TYPE_DC_ON, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_0),
            0x00000000,
            0x000FFFFF,
            &sdes[3]
        );

    // VGA DESCRIPTOR (this is only present so you can write string)
    create_descriptor(
            create_flags(FLAGS_GRANULARITY_OFF, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
            create_access(create_type(TYPE_EXEC_OFF, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_0),
            0x000B8000,
            0x00000FFF,
            &sdes[4]
        );

    // USER CODE DESCRIPTOR 
    create_descriptor(
            create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
            create_access(create_type(TYPE_EXEC_ON, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_3),
            0x00000000,
            0x000FFFFF,
            &sdes[5]
        );

    // USER DATA DESCRIPTOR
    create_descriptor(
            create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
            create_access(create_type(TYPE_EXEC_OFF, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_3),
            0x00000000,
            0x000FFFFF,
            &sdes[6]
        );

    // USER STACK DESCRIPTOR
    create_descriptor(
            create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
            create_access(create_type(TYPE_EXEC_OFF, TYPE_DC_ON, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_3),
            0x00000000,
            0x000FFFFF,
            &sdes[7]
        );
    load_gdt();
    reload_segments();
}
