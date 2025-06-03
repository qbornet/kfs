#pragma once
#include "lib/io_port.h"

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI   0x20

void pic_remap(uint8_t master_offset, uint8_t slave_offset);
void pic_send_eoi(uint8_t irq);
void pic_clear_mask(uint8_t irq);