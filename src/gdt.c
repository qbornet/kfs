#include "gdt.h"

sd_t    sdes[16];

static inline uint8_t   create_type(uint8_t exec, uint8_t dc, uint8_t rw, uint8_t access)
{
    return access | rw << 1 | dc << 2 | exec << 3;
}

static inline uint8_t   create_flags(uint8_t gran, uint8_t mode, uint8_t avl)
{
    return avl | 0 << 1 | mode << 2 | gran << 3;
}

static inline uint8_t   create_access(uint8_t type, uint8_t descriptor, uint8_t dpl)
{
    return (type & 0xf) | (descriptor & 0x1) << 4 | (dpl & 0x3) << 5 | (1 & 0x1) << 7;
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

static inline void    reload_segments(void)
{
    asm volatile(
            "jmp $0x08, $reload_cs\n\t"
            "reload_cs:\n\t"
            "mov $0x10, %ax\n\t"
            "mov %ax, %ds\n\t"
            "mov %ax, %es\n\t"
            "mov %ax, %fs\n\t"
            "mov $0x20, %ax\n\t"
            "mov %ax, %gs\n\t"
            "mov $0x18, %ax\n\t"
            "mov %ax, %ss\n\t"
        );
}

static inline void    load_gdt(void)
{
    struct gdt _gdt;

    _gdt.size = sizeof(sdes) - 1;
    _gdt.address = (uint32_t)&sdes;
    asm volatile ("lgdt %0" :: "m"(_gdt));
}

void    init_gdt(void)
{
    // NULL DESCRIPTOR
    create_descriptor(0, 0, 0, 0, &sdes[0]);

    // CODE DESCRIPTOR
    create_descriptor(
            create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
            create_access(create_type(TYPE_EXEC_ON, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_0),
            0x00400000,
            0x003FFFFF,
            &sdes[1]
        );
    
    // DATA DESCRIPTOR
    create_descriptor(
            create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
            create_access(create_type(TYPE_EXEC_OFF, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_0),
            0x00800000,
            0x003FFFFF,
            &sdes[2]

        );

    // STACK DESCRIPTOR
    create_descriptor(
            create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
            create_access(create_type(TYPE_EXEC_OFF, TYPE_DC_ON, TYPE_RW_ON, TYPE_A_OFF), ACCESS_DESCRIPTOR_TYPE_ON, ACCESS_DPL_RING_0),
            0x00F00000, // grows down so start at the end of the segment.
            0x003FFFFF,
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
    load_gdt();
    reload_segments();
}
