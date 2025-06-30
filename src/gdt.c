#include "gdt.h"

sd_t               g_sdes[16];
cpu_state_t       *g_cpu_state; // In the future should be an array of CPU
extern void        stack_top(void);

static inline void iomap_set(uint16_t port)
{
    uint32_t index = IOMAP_INDEX(port);
    uint8_t  bit = IOMAP_BIT(port);
    g_cpu_state->io_bitmap[index] |= (1 << bit);
}

static inline void iomap_clear(uint16_t port)
{
    uint32_t index = IOMAP_INDEX(port);
    uint8_t  bit = IOMAP_BIT(port);
    g_cpu_state->io_bitmap[index] &= ~(1 << bit);
}

static inline uint8_t iomap_test(uint16_t port)
{
    uint32_t index = IOMAP_INDEX(port);
    uint8_t  bit = IOMAP_BIT(port);
    return (g_cpu_state->io_bitmap[index] & (1 << bit)) ? 1 : 0;
}

static __always_inline void write_tss_entry(void)
{
    // save stack segment selector;
    g_cpu_state->tss.ss0 = 0x24;

    // call of stack_top to save kernel stack address
    g_cpu_state->tss.esp0 = (uint32_t)stack_top;

    // Authorize 0x3d4 and 0x3d5 port
    iomap_clear(0x3d4);
    iomap_clear(0x3d5);
}

static __always_inline void load_tss(void)
{
    asm volatile(".intel_syntax noprefix\n\t"
                 "mov ax, 0x40 \n\t"
                 "ltr ax\n\t"
                 ".att_syntax prefix\n\t"
                 :
                 :
                 : "ax", "memory");
}

static inline uint8_t
create_type(uint8_t exec, uint8_t dc, uint8_t rw, uint8_t access)
{
    return access | rw << 1 | dc << 2 | exec << 3;
}

static inline uint8_t create_flags(uint8_t gran, uint8_t mode, uint8_t avl)
{
    return avl | 0 << 1 | mode << 2 | gran << 3;
}

static inline uint8_t create_access(uint8_t type, uint8_t system, uint8_t dpl)
{
    return type | system << 4 | dpl << 5 | 1 << 7;
}

static inline void create_descriptor(uint8_t  flags,
                                     uint8_t  access,
                                     uint32_t addr,
                                     uint32_t limit,
                                     sd_ptr   segment_des)
{
    segment_des->start = (limit & 0xffff) | (addr & 0xffff) << 16;
    limit >>= 16;
    addr >>= 16;

    uint8_t low_address = (addr & 0xff);
    addr >>= 8;

    uint8_t high_address = (addr & 0xff);
    addr >>= 8;

    segment_des->end = low_address | (access & 0xff) << 8 | (limit & 0xf) << 16
                     | (flags & 0xf) << 20 | high_address << 24;
}

// reload only kernel descriptor other descriptor are loaded in gdtr but not
// used.
static __always_inline void reload_segments(void)
{
    __asm__ volatile(".intel_syntax noprefix\n\t"
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
                     : "ax", "memory");
}

static __always_inline void load_gdt(void)
{
    struct gdt gdt;
    gdt.size = sizeof(g_sdes) - 1;
    gdt.address = (uint32_t)&g_sdes;

    asm volatile("cld\n\t"
                 "mov %0, %%esi\n\t"
                 "mov %1, %%edi\n\t"
                 "mov %2, %%ecx\n\t"
                 "rep movsb\n\t"
                 :
                 : "r"(&g_sdes), "r"(0x800), "r"(sizeof(g_sdes))
                 : "esi", "edi", "ecx", "memory");
    asm volatile("lgdt %0\n\t" ::"m"(gdt));
}

static inline void test_user_mode_function(void)
{
    printk("Test in usermode\n");
    printk("CPL: %d\n", get_current_level_privilege());
    while (1) {
    }
}

__attribute__((naked, noreturn)) void jump_usermode(void)
{
    asm volatile(".intel_syntax noprefix\n\t"
                 "mov ax, 0x30 | 3\n\t"
                 "mov ds, ax\n\t"
                 "mov es, ax\n\t"
                 "mov fs, ax\n\t"
                 "push 0x30 | 3\n\t"
                 "push esp\n\t"
                 "pushf\n\t"
                 "push 0x28 | 3\n\t"
                 "push %0\n\t"
                 "iret\n\t"
                 ".att_syntax prefix\n\t"
                 :
                 : "r"(test_user_mode_function)
                 : "ax", "memory");
}

// TODO: Need to handle SMP.
void init_cpu_state(void)
{
    // zero tss and disable all io ports.
    memset(&g_cpu_state->tss, 0, sizeof(tss_segment_t));
    memset(g_cpu_state->io_bitmap, 0xFF, sizeof(g_cpu_state->io_bitmap));
    g_cpu_state->tss.iomap_base = (uint16_t)offsetof(cpu_state_t, io_bitmap);
    g_cpu_state->end_marker = 0xFF;
    write_tss_entry();
}

void init_gdt(void)
{
    // NULL DESCRIPTOR
    create_descriptor(0, 0, 0, 0, &g_sdes[0]);

    // KERNEL CODE DESCRIPTOR
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(
            create_type(TYPE_EXEC_ON, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF),
            ACCESS_DESCRIPTOR_TYPE_ON,
            ACCESS_DPL_RING_0),
        0x00000000,
        0x000FFFFF,
        &g_sdes[1]);

    // KERNEL DATA DESCRIPTOR
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(
            create_type(TYPE_EXEC_OFF, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF),
            ACCESS_DESCRIPTOR_TYPE_ON,
            ACCESS_DPL_RING_0),
        0x00000000,
        0x000FFFFF,
        &g_sdes[2]);

    // KERNEL STACK DESCRIPTOR
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(
            create_type(TYPE_EXEC_OFF, TYPE_DC_ON, TYPE_RW_ON, TYPE_A_OFF),
            ACCESS_DESCRIPTOR_TYPE_ON,
            ACCESS_DPL_RING_0),
        0x00000000,
        0x000FFFFF,
        &g_sdes[3]);

    // create_descriptor(0, 0, 0, 0, &g_sdes[4]);
    // VGA DESCRIPTOR (this is only present so you can write string)
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_OFF, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(
            create_type(TYPE_EXEC_OFF, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF),
            ACCESS_DESCRIPTOR_TYPE_ON,
            ACCESS_DPL_RING_3),
        0x000B8000,
        0x00000FFF,
        &g_sdes[4]);

    // USER CODE DESCRIPTOR
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(
            create_type(TYPE_EXEC_ON, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF),
            ACCESS_DESCRIPTOR_TYPE_ON,
            ACCESS_DPL_RING_3),
        0x00000000,
        0x000FFFFF,
        &g_sdes[5]);

    // USER DATA DESCRIPTOR
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(
            create_type(TYPE_EXEC_OFF, TYPE_DC_OFF, TYPE_RW_ON, TYPE_A_OFF),
            ACCESS_DESCRIPTOR_TYPE_ON,
            ACCESS_DPL_RING_3),
        0x00000000,
        0x000FFFFF,
        &g_sdes[6]);

    // USER STACK DESCRIPTOR
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_ON, FLAGS_MODE_ON, FLAGS_AVL_64_OFF),
        create_access(
            create_type(TYPE_EXEC_OFF, TYPE_DC_ON, TYPE_RW_ON, TYPE_A_OFF),
            ACCESS_DESCRIPTOR_TYPE_ON,
            ACCESS_DPL_RING_3),
        0x00000000,
        0x000FFFFF,
        &g_sdes[7]);

    // TASK STATE DESCRIPTOR
    uint32_t base = (uint32_t)&g_cpu_state->tss;
    uint32_t limit = (uint32_t)&g_cpu_state->end_marker - base + 1;
    create_descriptor(
        create_flags(FLAGS_GRANULARITY_OFF, FLAGS_MODE_OFF, FLAGS_AVL_64_OFF),
        create_access(
            create_type(TYPE_EXEC_ON, TYPE_DC_OFF, TYPE_RW_OFF, TYPE_A_ON),
            ACCESS_DESCRIPTOR_TYPE_OFF,
            ACCESS_DPL_RING_0),
        base,
        limit,
        &g_sdes[8]);

    load_gdt();
    load_tss();
    reload_segments();
}
