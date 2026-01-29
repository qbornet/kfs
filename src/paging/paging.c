#include <paging/paging.h>
#include <paging/pfn.h>
#include <paging/phys_alloc.h>

typedef union {
    pde_t    directory;
    uint32_t representation;
} debug_t;

static uint32_t                                g_start_virt_mem = 0xc0400000;

// Kernel Space table for memory.
__attribute__((aligned(4096))) static uint32_t g_page_table_kernel_space[1024];

// User Space table for memory.
__attribute__((aligned(4096))) static uint32_t g_page_table_user_space[1024];

/* flush translation lookaside buffer (tlb) meaning that entry inside tlb cache,
 * (store pte) will be flushed so not saved. This avoid filling,
 * to much of the cache memory and allow us better control over the MMU caching,
 * system. Note this will flush all address containes in that pages of the
 * virtual address.
 *
 * @vaddr: Virtual Address
 * */
static __always_inline void                    flush_tlb(uint32_t vaddr)
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
    printk(
        "all phys value: %p, shifted 12: 0x%x\n", phys, (uint32_t)phys >> 12);
    return (uint32_t)phys >> 12;
}

// destroy a page table based on the vaddr that you provide.
// the vaddr need to be in the range of the page table to be destroy.
void destroy_memory_page(void *vaddr)
{
    uint32_t pdindex = PDE_INDEX(vaddr);
    uint32_t ptindex = PTE_INDEX(vaddr);
    pde_t   *v_pde = (pde_t *)P2V(g_page_directory);
    if (v_pde[pdindex].present == 0)
        return; // Error no page directory entry index found;

    pte_t *v_pte = (pte_t *)P2V((uint32_t)(v_pde[pdindex].address << 12));
    if (v_pte[ptindex].present == 0)
        return; // Error no page table entry index found;

    memset((void *)vaddr, 0, FRAME_SIZE);
    flush_tlb((uint32_t)vaddr);
    pfree((page_frame_t)((uint32_t)v_pte[ptindex].address << 12));
    memset(&v_pde[pdindex], 0, sizeof(pde_t));
    memset(&v_pte[ptindex], 0, sizeof(pte_t));
}

void *get_memory_page(page_frame_t frame, uint32_t vaddr)
{
    printk("frame passed: %p\n", frame);
    uint32_t ptindex = PTE_INDEX(vaddr);
    uint32_t pdindex = PDE_INDEX(vaddr);
    printk("ptindex: %u, pdindex: %u\n", ptindex, pdindex);
    // Get the g_page_directory
    pde_t  *v_pde = (pde_t *)P2V(g_page_directory);
    debug_t v_pde_0 = { .directory = v_pde[0] };
    debug_t v_pde_768 = { .directory = v_pde[768] };
    debug_t v_pde_769 = { .directory = v_pde[769] };
    printk("v_pde: %p\nvalue hold at 0: 0x%x, at 768: 0x%x, at 769: 0x%x\n",
           v_pde,
           v_pde_0.representation,
           v_pde_768.representation,
           v_pde_769.representation);

    pte_t *v_pte = (pte_t *)((uint32_t)&g_page_table_kernel_space);
    if (v_pte[ptindex].present == 0) {
        v_pte[ptindex].rw = 1;
        v_pte[ptindex].address = get_address_value(frame);
        v_pte[ptindex].present = 1;
    } else {
        printk("Already mapped skipping...\n");
        return NULL;
    }

    v_pde[pdindex].rw = 1;
    v_pde[pdindex].address = get_address_value((void *)V2P(v_pte));
    v_pde[pdindex].present = 1;
    v_pde_768.directory = v_pde[768];
    v_pde_769.directory = v_pde[pdindex];
    printk("v_pde: %p\nvalue hold at 0: 0x%x, at 768: 0x%x, at 769: 0x%x\n",
           v_pde,
           v_pde_0.representation,
           v_pde_768.representation,
           v_pde_769.representation);
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
    init_pfn_db(size);
    init_frame_page(base_addrs, size);
    printk("Starting kalloc_frame()\n");
    page_frame_t frame = (page_frame_t)pmalloc(10);
    printk("frame_present: %p\n", frame);
    printk("get_memory_page()\n");
    (void)g_page_table_user_space;
    void *vaddr = get_memory_page(frame, g_start_virt_mem);
    if (vaddr) {
        printk("frame: %p, vaddr: %p\n", frame, vaddr);
        memcpy(vaddr, "toto", 4);
        printk("%p:[%s]\n", vaddr, (char *)vaddr);
        destroy_memory_page(vaddr);
    } else {
        printk("Error when getting new page\n");
    }
}
