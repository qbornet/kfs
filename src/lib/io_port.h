#ifndef IO_PORT_H
#define IO_PORT_H

#include <stdint.h>

/**
 * Read a byte from the given I/O port.
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile(
        ".intel_syntax noprefix\n\t"
        "in al, dx\n\t"
        ".att_syntax prefix"
        : "=a"(data)
        : "d"(port)
    );
    return data;
}

/**
 * Write a byte to the given I/O port.
 */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile(
        ".intel_syntax noprefix\n\t"
        "out dx, al\n\t"
        ".att_syntax prefix"
        :
        : "a"(val), "d"(port)
    );
}

/**
 * Read a word (16-bit) from the given I/O port.
 */
static inline uint16_t inw(uint16_t port) {
    uint16_t data;
    asm volatile(
        ".intel_syntax noprefix\n\t"
        "in ax, dx\n\t"
        ".att_syntax prefix"
        : "=a"(data)
        : "d"(port)
    );
    return data;
}

/**
 * Write a word (16-bit) to the given I/O port.
 */
static inline void outw(uint16_t port, uint16_t val) {
    asm volatile(
        ".intel_syntax noprefix\n\t"
        "out dx, ax\n\t"
        ".att_syntax prefix"
        :
        : "a"(val), "d"(port)
    );
}

/**
 * Read a double-word (32-bit) from the given I/O port.
 */
static inline uint32_t inl(uint16_t port) {
    uint32_t data;
    asm volatile(
        ".intel_syntax noprefix\n\t"
        "in eax, dx\n\t"
        ".att_syntax prefix"
        : "=a"(data)
        : "d"(port)
    );
    return data;
}

/**
 * Write a double-word (32-bit) to the given I/O port.
 */
static inline void outl(uint16_t port, uint32_t val) {
    asm volatile(
        ".intel_syntax noprefix\n\t"
        "out dx, eax\n\t"
        ".att_syntax prefix"
        :
        : "a"(val), "d"(port)
    );
}

#endif