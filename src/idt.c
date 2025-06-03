#include "idt.h"

#define IDT_MAX_DESCRIPTORS 2

__attribute__((aligned(0x10))) static idt_entry_t g_idt[256];

static idtr_t                                     g_idtr;

__attribute__((noreturn)) void                    exception_handler(void)
{
    // Disable interrupts and halt. The CPU will sit in HLT forever,
    // but the compiler doesn’t know HLT won’t return. So add a
    // “builtin_unreachable” or infinite loop after it:
    __asm__ volatile("cli; hlt");

    // Tell the compiler “we never get here”:
    __builtin_unreachable();
}

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
    idt_entry_t *descriptor = &g_idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08; // this value can be whatever offset your
                                  // kernel code selector is in your GDT
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

static bool  g_vectors[IDT_MAX_DESCRIPTORS];

extern void *g_isr_stub_table[];

void         idt_init()
{
    g_idtr.base = (uintptr_t)&g_idt[0];
    g_idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for(uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, g_isr_stub_table[vector], 0x8E);
        g_vectors[vector] = true;
    }

    __asm__ volatile("lidt %0"
                     :
                     : "m"(g_idtr)); // load the new IDT
    __asm__ volatile("sti");         // set the interrupt flag
}
