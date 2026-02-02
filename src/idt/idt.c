#include <idt/idt.h>
extern void                                       isr14();

__attribute__((aligned(0x10))) static idt_entry_t g_idt[IDT_MAX_DESCRIPTORS];
static idtr_t                                     g_idtr;

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
    idt_entry_t *descriptor = &g_idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

void init_idt(void)
{
    g_idtr.base = (uintptr_t)&g_idt[0];
    g_idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;
    idt_set_descriptor(14, isr14, 0x8E);

    __asm__ volatile("lidt %0"
                     :
                     : "m"(g_idtr));
}
