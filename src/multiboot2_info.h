#ifndef MULTIBOOT2_INFO_H
#define MULTIBOOT2_INFO_H
#include <multiboot2.h>
#include <stdint.h>

/*
 * Reference here for the tag system and handling from the grub2
 * https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#kernel_002ec
 *
 * This return the max value of memory available passed via multiboot2,
 * information.
 *
 * @mbi: Multiboot Information pointer (data structure of multiboot2)
 * */
uint32_t get_memory_max_value(uint32_t mbi);
#endif
