const std = @import("std");
const mem = std.mem;

const COM1_PORT = 0x3F8;
 
pub inline fn outb(value: u8, port: u16) void {
    return asm volatile(
        \\outb %[value], %[port]
        :
        : [value] "{al}" (value),
          [port] "{dx}" (port),
    );
}

pub inline fn inb(port: u16) u8 {
    return asm volatile(
        \\inb %[port], %[ret]
        : [ret] "={al}" (-> u8),
        : [port] "{dx}" (port),
    );
}

pub inline fn putchar(c: u8) void {
    // Wait for COM1_PORT to be available and print character.
    while ((inb(COM1_PORT + 5) & 0x20) == 0) {} 
    outb(c, COM1_PORT);
}

pub inline fn putstr(str: []const u8) void {
    for (str) |c| {
        putchar(c);
    }
}

pub fn init() void {
    outb(0x00, COM1_PORT + 1); // Disable interrupts
    outb(0x80, COM1_PORT + 3); // Enable DLAB (set baud rate divisor)
    outb(0x01, COM1_PORT + 0); // Divisor low byte (115 200 baud rate)
    outb(0x00, COM1_PORT + 1); // Divisor high byte
    outb(0x03, COM1_PORT + 3); // 8N1 (no parity, 1 stop bit)
    outb(0xC7, COM1_PORT + 2); // Enable FIFO
    outb(0x0B, COM1_PORT + 4); // Enable IRQs, RTS/DSR
}
