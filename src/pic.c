#include "pic.h"

static inline void io_wait(void)
{
    outb(0x80, 0);
}

void pic_remap(uint8_t off1, uint8_t off2)
{
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    /* starts the initialization sequence */
    outb(PIC1_CMD, 0x11);
    io_wait();
    outb(PIC2_CMD, 0x11);
    io_wait();

    outb(PIC1_DATA, off1);
    io_wait(); // ICW2
    outb(PIC2_DATA, off2);
    io_wait();

    outb(PIC1_DATA, 0x04);
    io_wait(); // ICW3 – tell Master about slave at IRQ2
    outb(PIC2_DATA, 0x02);
    io_wait();

    outb(PIC1_DATA, 0x01);
    io_wait(); // ICW4 – 8086 mode
    outb(PIC2_DATA, 0x01);
    io_wait();

    /* restore masks */
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void pic_send_eoi(uint8_t irq)
{
    if(irq >= 8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

void pic_clear_mask(uint8_t irq)
{
    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t  val = inb(port) & ~(1 << (irq & 7));
    outb(port, val);
}