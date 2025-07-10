#ifndef PAGING_H
#define PAGING_H
#include "../lib/io.h"
#include "../lib/mem.h"
#include "../lib/shared/defs.h"
#include <stdbool.h>
#include <stdint.h>

// For page directory and table present in memory is ON or OFF
#define PF_PRESENT_ON      1
#define PF_PRESENT_OFF     0

// For page directory and table Read only ON, Read & Write OFF
#define PF_RW_ON           1
#define PF_RW_OFF          0

// For page directory and table user (RING0) OFF, user is (RING>1) ON
#define PF_USER_ON         1
#define PF_USER_OFF        0

#define PTE_INDEX(addr)    ((addr >> 12) & 0x3FF)
#define PDE_INDEX(addr)    ((addr >> 22) & 0x3FF)
#define BEGIN_KERNEL_SPACE 0xC0000000
#define BEGIN_USER_SPACE   0x80000000

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

/*
 * pagging global structure for virtual addressing
 * */

// Init paging
void                      init_paging(void);
uint32_t                 *get_virtual_address(void);

extern uint32_t           g_end;

#endif
