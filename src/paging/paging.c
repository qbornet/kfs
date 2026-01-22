#include <paging/paging.h>
#include <paging/phys_alloc.h>
uint8_t                     g_current_page_table = 0;
static pte_t                g_page_table_kernel_space[1024];

/* flush translation lookaside buffer (tlb) meaning that entry inside tlb cache,
 * (store pte) will be flushed so not saved. This avoid filling,
 * to much of the cache memory and allow us better control over the MMU caching,
 * system. Note this will flush all address containes in that pages of the
 * virtual address.
 *
 * @vaddr: Virtual Address
 * */
static __always_inline void flush_tlb(uint32_t vaddr)
{
    asm volatile("invlpg (%0)" ::"r"(vaddr)
                 : "memory");
}

// set cr3 page directory in register
static __always_inline void set_cr3(uint32_t page_directory)
{
    asm volatile(".intel_syntax noprefix\n\t"
                 "mov eax, %0\n\t"
                 "mov cr3, eax\n\t"
                 ".att_syntax prefix\n\t" ::"r"(page_directory)
                 : "eax");
}

static __always_inline uint32_t get_address_value(void *phys)
{
    return (uint32_t)phys >> 12;
}

static __always_inline uint8_t is_directory_full(uint16_t pdindex)
{
    pde_t *v_pde = (pde_t *)P2V(g_page_directory);
    if (v_pde[pdindex].present == 0) return 0;
    pte_t *v_pte = (pte_t *)((uint32_t)v_pde[pdindex].address);

    for (uint16_t i = 0; i < 1024; i++) {
        if (v_pte[i].present == 0) return 0;
    }
    return 1;
}

// destroy a page table based on the index that you provide.
void destroy_page_table(uint16_t pdindex, uint16_t ptindex)
{
    uint32_t vaddr = pdindex << 22 | ptindex << 12;
    pte_t    page_table_entry = g_page_table_kernel_space[ptindex];

    if (page_table_entry.present != 1) return; // Error: not in memory
    memset((void *)vaddr, 0, FRAME_SIZE);
    flush_tlb(vaddr);
    kfree_frame((page_frame_t)((uint32_t)page_table_entry.address));
    memset(&page_table_entry, 0, sizeof(pte_t));
}

// will get a page clean page table based on the index that you provide.
pte_t *get_page_table(uint16_t ptindex)
{
    printk("g_page_table_kernel_space: %p\n", g_page_table_kernel_space);
    pte_t page_table_entry = g_page_table_kernel_space[ptindex];
    if (page_table_entry.present == 1) return NULL;
    printk("page_table_entry found: %p\n", &g_page_table_kernel_space[ptindex]);
    return &g_page_table_kernel_space[ptindex];
}

int get_free_page_directory()
{
    pde_t *v_pde = (pde_t *)P2V(g_page_directory);
    for (uint32_t i = 0; i < 1024; i++) {
        // give page directory entry for only not present in memory and not
        // accessed (dirty) entry or only not accessed page directory entry.
        if ((!v_pde[i].present && !v_pde[i].accessed) || !v_pde[i].accessed)
            return i;
    }
    return -1;
}

void *get_memory_page(page_frame_t frame, uint32_t vaddr)
{
    // Get index from virtual address that you want to map.
    uint16_t pdindex = (uint16_t)vaddr >> 22;
    uint16_t ptindex = (uint16_t)vaddr >> 12 & 0x3FF;
    if (is_directory_full(pdindex)) {
        printk("error is full\n");
        // Error: directory index passed as full page_table already.
        return NULL;
    }

    // Get the phyiscal address to virtual so we can modify pde and pte.
    pde_t *v_pde = (pde_t *)P2V(g_page_directory);
    pte_t *v_pte = get_page_table(ptindex);
    if (v_pte == NULL) {
        printk("v_pte is null\n");
        // Error: ptindex is already map.
        return NULL;
    }
    printk("v_pde: %p, v_pte:  %p\n", v_pde, v_pte);

    // Set read write access for both
    v_pte[ptindex].read_write = 1;
    v_pde[pdindex].read_write = 1;

    // Set page_table physical address to page directory.
    v_pde[pdindex].address = get_address_value((void *)&v_pte[ptindex]);

    // Page_frame added to v_pte so now we have physical memory linked to the
    // virtual one.
    v_pte[ptindex].address = (uint32_t)frame;

    // Set present to say that it's present in physical memory.
    v_pte[ptindex].present = 1;
    v_pde[pdindex].present = 1;
    set_cr3((uint32_t)g_page_directory);
    return (void *)vaddr;
}

/*
page_table_entry_t *get_page_table_entry(void *vaddr)
{
    uint32_t pte_index = PTE_INDEX((uintptr_t)vaddr);
    return &g_page_table[pte_index];
}

page_directory_entry_t *get_page_directory_entry(void *vaddr)
{
    uint32_t pde_index = PDE_INDEX((uintptr_t)vaddr);
    return &g_page_directory[pde_index];
}
*/

void init_paging(uint32_t base_addrs, uint32_t size)
{
    printk("Memory available: %x B\n", size);
    init_frame_page(base_addrs, size);
    printk("Starting kalloc_frame()\n");
    page_frame_t frame = kalloc_frame();
    printk("frame_present: %p\n", frame);
    printk("get_memory_page()\n");
    void *vaddr = get_memory_page(frame, VIRTUAL_BASE + KERNEL_MAP_SIZE);
    printk("frame: %p, vaddr: %p\n", frame, vaddr);
    // memcpy(vaddr, "toto", 4);
    // printk("%p:[%s]\n", vaddr, (char *)vaddr);
}
