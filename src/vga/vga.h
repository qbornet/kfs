#ifndef VGA_H
#define VGA_H
#include "../lib/string.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// enum text mode vga color.
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GREY,
    VGA_COLOR_DARK_GREY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_LIGHT_BROWN,
    VGA_COLOR_WHITE,
};

// Get a vga color.
// Blink param if set to 1 is making cursor blink.
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg, int blink);

// Get terminal row global variable value.
// (terminal_row variable is not extern)
int     get_terminal_row(void);

// Get terminal column global variable value.
// (terminal_column variable is not extern)
int     get_terminal_column(void);

// Set terminal row global variable.
// (terminal_row variable is not extern)
void    set_terminal_row(int num);

// Set terminal column global variable.
// (terminal_column variable is not extern)
void    set_terminal_column(int num);

// Add terminal row global variable.
// (terminal_row variable is not extern)
void    add_terminal_row(int num);

// Add terminal column global variable.
// (terminal_column variable is not extern)
void    add_terminal_column(int num);

// Set color to the current terminal_color.
void    terminal_setcolor(uint8_t color);

// Terminal initialize set blank through all the memory for vga text mode.
void    terminal_initialize(void);

// Write a character to the current position based on terminal_row and
// terminal_column variable.
void    terminal_putchar(char c);

// Write a string to the current position based on terminal_row and
// terminal_column variable.
void    terminal_writestring(const char *data, uint8_t color);
#endif
