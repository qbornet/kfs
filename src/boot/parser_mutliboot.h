#ifndef PARSER_MUTLIBOOT_H
#define PARSER_MUTLIBOOT_H
#include <multiboot2.h>
#include <multiboot2_info.h>
#include <stddef.h>
typedef struct s_boot_info {
    uint32_t max_memory;
    uint32_t mem_map_count;
} __attribute__((packed)) boot_info_t;

boot_info_t              *parse_multiboot(uint32_t magic, uint32_t mbi_addr);
#endif
