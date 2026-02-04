#include <serial/serial.h>

void serial_init(void)
{
    outb(0x00, COM1_PORT + 1); // Disable interrupts
    outb(0x80, COM1_PORT + 3); // Enable DLAB (set baud rate divisor)
    outb(0x01, COM1_PORT + 0); // Divisor low byte (115 200 baud rate)
    outb(0x00, COM1_PORT + 1); // Divisor high byte
    outb(0x03, COM1_PORT + 3); // 8N1 (no parity, 1 stop bit)
    outb(0xC7, COM1_PORT + 2); // Enable FIFO
    outb(0x0B, COM1_PORT + 4); // Enable IRQs, RTS/DSR
}

void serial_putchar(char c)
{
    // wait for com1 to be available and print character
    while ((inb(COM1_PORT + 5) & 0x20) == 0);
    outb(c, COM1_PORT);
}

void serial_write(const char *str)
{
    for (int i = 0; str[i]; i++) {
        serial_putchar(str[i]);
    }
}
