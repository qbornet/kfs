#ifndef PAGE_DIRECTORY_ENTRY_H
#define PAGE_DIRECTORY_ENTRY_H
#include <stdint.h>

/*
 * Page directory entry point to page table entry and define page table entry
 * rights
 * */
typedef struct s_page_directory_entry {
    uint8_t  present : 1;
    uint8_t  read_write : 1;
    uint8_t  user_supervisor : 1;
    uint8_t  write_through : 1;
    uint8_t  cache_disable : 1;
    uint8_t  accessed : 1;
    uint8_t  ignored0 : 1;
    uint8_t  page_size : 1;
    uint8_t  ignored1 : 4;
    uint32_t address : 20;
} __attribute__((packed)) page_directory_entry_t;

#endif
