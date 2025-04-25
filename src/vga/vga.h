#ifndef VGA_H
# define VGA_H
# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>
# define VGA_WIDTH   80
# define VGA_HEIGHT  25
# define VGA_MEMORY 0xB8000

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

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg, int blink); 
void    terminal_initialize(void);
void    terminal_writestring(const char* data, uint8_t color);
#endif
