#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define NBITS           32
#define TABLE_SIZE      1024
#define PAGE_SIZE       0x1000
#define PAGE_ALIGN_MASK 0xFFFFF000

typedef struct page_s {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t unused : 7;
    uint32_t frame : 20;
} page_t;

typedef struct page_table_s {
    page_t pages[TABLE_SIZE];
} page_table_t;

typedef struct page_directory_s {
    page_table_t *tables[TABLE_SIZE];
    uint32_t      tables_physical[TABLE_SIZE];
    uint32_t      physical_address;
} page_directory_t;

void    initialise_paging();

void    switch_to_page_directory(page_directory_t *dir);

page_t *get_page(uint32_t address, int make, page_directory_t *dir);

void    alloc_frame(page_t *page, int is_kernel, int is_writeable);

void    free_frame(page_t *page);

#endif
