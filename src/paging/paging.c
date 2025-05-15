#include "paging.h"
#include "kernelHeap.h"

#include "vga/vga.h"

#define FRAME_NUMBER(a) ((a) >> 12)  // div PAGE_SIZE
#define FRAME_INDEX(n)  ((n) >> 5)   // div NBITS
#define FRAME_OFFSET(n) ((n) & 0x1F) // mod NBITS

extern uint32_t   g_placement_address;
extern heap_t    *g_kheap;

uint32_t         *g_frames;
uint32_t          g_nframes;

page_directory_t *g_kernel_directory = 0;

page_directory_t *g_current_directory = 0;

static void       set_frame(uint32_t frame_address)
{
    uint32_t frame = FRAME_NUMBER(frame_address);
    uint32_t index = FRAME_INDEX(frame);
    uint32_t offset = FRAME_OFFSET(frame);
    uint32_t mask = 1u << offset;
    g_frames[index] |= mask;
}

static void clear_frame(uint32_t frame_address)
{
    uint32_t frame = FRAME_NUMBER(frame_address);
    uint32_t index = FRAME_INDEX(frame);
    uint32_t offset = FRAME_OFFSET(frame);
    uint32_t mask = 1u << offset;
    g_frames[index] &= ~mask;
}

static inline uint32_t test_frame(uint32_t frame_address)
{
    uint32_t frame = FRAME_NUMBER(frame_address);
    uint32_t index = FRAME_INDEX(frame);
    uint32_t offset = FRAME_OFFSET(frame);
    return g_frames[index] & (1u << offset);
}

static int32_t find_first_free_frame(void)
{
    uint32_t len = FRAME_INDEX(g_nframes);
    for(uint32_t i = 0; i < len; i++) {
        uint32_t word = g_frames[i];
        if(word != 0xFFFFFFFF) {
            uint32_t free_bits = ~word;
            uint32_t offset = __builtin_ctz(free_bits);
            return i * NBITS + offset;
        }
    }
    return (int32_t)-1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if(page->frame) return;

    int32_t index = find_first_free_frame();
    // if (index == -1)
    // Add a panic

    set_frame(index * PAGE_SIZE);
    int32_t word = index / NBITS;
    int32_t bit = index % NBITS;
    g_frames[word] |= 1u << bit;
    page->present = 1;
    page->rw = is_writeable;
    page->user = !is_kernel;
    page->frame = index;
}

void free_frame(page_t *page)
{
    uint32_t frame;

    frame = page->frame;
    if(!frame) return;

    clear_frame(frame);
    page->frame = 0;
}

void initialise_paging()
{
    uint32_t tmp;
    uint32_t mem_size = 0x10000000;
    uint32_t placement_address;

    placement_address = (uint32_t)&end;
    g_nframes = FRAME_NUMBER(mem_size);
    g_frames = (uint32_t *)kmalloc(FRAME_INDEX(g_nframes));
    memset((uint8_t *)g_frames, 0, FRAME_INDEX(g_nframes));
    tmp = kmalloc_aligned(sizeof(page_directory_t));
    memset((uint8_t *)tmp, 0, sizeof(page_directory_t));
    g_kernel_directory = (page_directory_t *)tmp;
    g_current_directory = g_kernel_directory;

    for(int i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE;
        i += PAGE_SIZE) {
        get_page(i, 1, kernel_directory);
    }

    for(uint32_t i = 0; i < placement_address; i += PAGE_SIZE) {
        page_t *p = get_page(i, 1, g_kernel_directory);
        alloc_frame(p, 0, 0);
    }

    for(int i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE;
        i += PAGE_SIZE) {
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
    }

    switch_to_page_directory(g_kernel_directory);

    kheap = create_heap(
        KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
    uint32_t frame = address / PAGE_SIZE;
    uint32_t table_idx = frame / TABLE_SIZE;
    uint32_t page_idx = frame % TABLE_SIZE;

    if(dir->tables[table_idx]) {
        return &dir->tables[table_idx]->pages[page_idx];
    } else if(make) {
        uint32_t physical_address;
        uint32_t virtual_address;

        virtual_address
            = kmalloc_alignedp(sizeof(page_table_t), &physical_address);
        memset((uint8_t *)virtual_address, 0, PAGE_SIZE);
        dir->tables[table_idx] = (page_table_t *)virtual_address;
        dir->tables_physical[table_idx] = physical_address | 0x7;
        return &dir->tables[table_idx]->pages[page_idx];
    }
    return 0;
}

void switch_to_page_directory(page_directory_t *dir)
{
    g_current_directory = dir;
    asm volatile(".intel_syntax noprefix\n\t"
                 "mov eax, %0\n\t"
                 "mov cr3, eax\n\t"
                 "mov eax, cr0\n\t"
                 "or  eax, %1\n\t"
                 "mov cr0, eax\n\t"
                 ".att_syntax prefix"
                 :
                 : "r"(&dir->tables_physical), "i"(0x80000000)
                 : "eax", "memory");
}
