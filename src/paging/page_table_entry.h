#ifndef PAGE_TABLE_ENTRY_H
#define PAGE_TABLE_ENTRY_H
// #include "phys_alloc.h"
#include <stdint.h>

/*
 * Page table entry use to define rights and point to page
 * */
typedef struct s_page_table_entry {
    uint8_t  present : 1;
    uint8_t  read_write : 1;
    uint8_t  user_supervisor : 1;
    uint8_t  write_through : 1;
    uint8_t  cache_disable : 1;
    uint8_t  accessed : 1;
    uint8_t  dirty : 1;
    uint8_t  page_attribute : 1;
    uint8_t  global : 1;
    uint8_t  ignored : 3;
    uint32_t address : 20;
} __attribute__((packed)) page_table_entry_t;

#endif
