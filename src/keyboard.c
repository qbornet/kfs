#include "keyboard.h"

static const char KEYMAP[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' ', 0, /* the rest are
                                                                unused for now
                                                              */
};

void keyboard_isr(void)
{
    uint8_t sc = inb(0x60);

    if(!(sc & 0x80)) { /* key press (ignore releases) */
        char c = KEYMAP[sc];
        if(c) terminal_putchar(c);
    }

    pic_send_eoi(1);
}

void keyboard_init(void)
{
    /* vector 0x21 (33) is IRQ1 after remap */
    extern void isr_keyboard_stub(void);
    idt_set_descriptor(0x21, isr_keyboard_stub, 0x8E);

    pic_clear_mask(1); /* already done in kernel_main, harmless here */
}