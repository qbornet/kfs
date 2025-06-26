#ifndef PAGING_H
# define PAGING_H
# include "lib/shared.h"
# include "lib/mem.h"
# include <stdint.h>
# include <stdbool.h>


/*
 * Page directory entry point to page table entry and define page table entry rights
 * */
typedef struct s_page_directory_entry {
    uint8_t     present         : 1;
    uint8_t     read_write      : 1;
    uint8_t     user_supervisor : 1;
    uint8_t     write_through   : 1;
    uint8_t     cache_disable   : 1;
    uint8_t     accessed        : 1;
    uint8_t     ignored0        : 1;
    uint8_t     page_size       : 1;
    uint8_t     ignored1        : 4;
    uint32_t    address         : 20;
} __attribute__((packed)) page_directory_entry_t;

/*
 * Page table entry use to define rights and point to page
 * */
typedef struct s_page_table_entry {
    uint8_t     present         : 1;
    uint8_t     read_write      : 1;
    uint8_t     user_supervisor : 1;
    uint8_t     write_through   : 1;
    uint8_t     cache_disable   : 1;
    uint8_t     accessed        : 1;
    uint8_t     dirty           : 1;
    uint8_t     page_attribute  : 1;
    uint8_t     global          : 1;
    uint8_t     ignored         : 4;
    uint32_t    address         : 20;
} __attribute__((packed)) page_table_entry_t;

// Create basic page.
typedef unsigned char page_t[4096];

/*
 * pagging global structure for virtual addressing
 * */
extern page_directory_entry_t   g_page_directory[1024];
extern page_table_entry_t       g_page_table[1024];

#endif
