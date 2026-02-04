#ifndef PARSER_MUTLIBOOT_H
#define PARSER_MUTLIBOOT_H
#include <multiboot2.h>
#include <multiboot2_info.h>
#include <stddef.h>
typedef struct s_mmap_info {
    uint32_t low_len;
    uint32_t base_low;
    uint32_t high_len;
    uint32_t base_high;
    uint32_t type;
} __attribute__((packed)) mmap_info_t;

typedef struct s_boot_info {
    uint32_t    max_memory;
    uint8_t     mmap_max_size;
    mmap_info_t mmap_info[32];
} __attribute__((packed)) boot_info_t;

boot_info_t              *parse_multiboot(uint32_t magic, uint32_t mbi_addr);
#endif
