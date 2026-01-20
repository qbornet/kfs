#ifndef SERIAL_H
#define SERIAL_H
#define COM1_PORT 0x3F8
#include <lib/shared/io.h>

void serial_putchar(char c);
void serial_write(const char *str);
void serial_init(void);
#endif
