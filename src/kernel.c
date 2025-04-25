#include "vga/vga.h"

void kernel_main(void)
{
	terminal_initialize();
	terminal_writestring("42", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK, 0));
    terminal_writestring("|", vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_GREEN, 0));
    terminal_writestring("|", vga_entry_color(VGA_COLOR_CYAN, VGA_COLOR_RED, 0));
}
