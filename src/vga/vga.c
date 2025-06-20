#include "vga.h"

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg, int blink)
{
    return fg | bg << 4 | blink << 7;
}

size_t    g_terminal_row;
size_t    g_terminal_column;
uint8_t   g_terminal_color;
uint16_t *g_terminal_buffer = (uint16_t *)VGA_MEMORY;

void      terminal_initialize(void)
{
    g_terminal_row = 0;
    g_terminal_column = 0;
    g_terminal_color
        = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK, 0);

    for(size_t y = 0; y < VGA_HEIGHT; y++) {
        for(size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            g_terminal_buffer[index] = vga_entry(' ', g_terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    g_terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = (y * VGA_WIDTH + x) * 2;
    uint16_t     value = vga_entry(c, color);

    asm volatile("movw %0,  %%gs:(%1)" ::"r"(value), "r"(index)
                 : "memory");
}

void set_terminal_column(int num)
{
    if(num > VGA_WIDTH) return;
    g_terminal_column = num;
}

void set_terminal_row(int num)
{
    if(num > VGA_HEIGHT) return;
    g_terminal_row = num;
}

void add_terminal_column(int num)
{
    if(g_terminal_column + num == VGA_WIDTH) g_terminal_column = 0;
    g_terminal_column += num;
}

void add_terminal_row(int num)
{
    if(g_terminal_row + num == VGA_HEIGHT) g_terminal_row = 0;
    g_terminal_row += num;
}

int get_terminal_row(void)
{
    return g_terminal_row;
}

int get_terminal_column(void)
{
    return g_terminal_column;
}

void terminal_putchar(char c)
{
    if(c == '\n') {
        g_terminal_row++;
        if(g_terminal_row == VGA_HEIGHT) g_terminal_row = 0;
        g_terminal_column = 0;
        return;
    }
    terminal_putentryat(c, g_terminal_color, g_terminal_column, g_terminal_row);
    if(++g_terminal_column == VGA_WIDTH) {
        g_terminal_column = 0;
        if(++g_terminal_row == VGA_HEIGHT) g_terminal_row = 0;
    }
}

void terminal_write(const char *data, size_t size)
{
    for(size_t i = 0; i < size; i++) {
        if(data[i] == '\n') terminal_putchar('\r');
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char *data, uint8_t color)
{
    if(color != 0) {
        terminal_setcolor(color);
    } else {
        terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK, 0));
    }
    terminal_write(data, strlen(data));
}
