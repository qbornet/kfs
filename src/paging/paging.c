#include <lib/kmalloc.h>
#include <paging/paging.h>
#include <paging/pfn.h>

typedef union {
    pde_t    directory;
    uint32_t representation;
} debug_t;

uint32_t g_kernel_start_virt_mem = KERNEL_START_VIRT_MEM;
uint32_t g_user_start_virt_mem = USER_START_VIRT_MEM;

// Kernel Space table for memory.
__attribute__((aligned(4096))) uint32_t g_page_table_kernel_space[1024];

// User Space table for memory.
__attribute__((aligned(4096))) uint32_t g_page_table_user_space[1024];

/* flush translation lookaside buffer (tlb) meaning that entry inside tlb cache,
 * (store pte) will be flushed so not saved. This avoid filling,
 * to much of the cache memory and allow us better control over the MMU caching,
 * system. Note this will flush all address containes in that pages of the
 * virtual address.
 *
 * @vaddr: Virtual Address
 * */
static __always_inline void             flush_tlb(uint32_t vaddr)
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

void *get_memory_page(page_frame_t frame, uint32_t vaddr, uint8_t flags)
{
    uint32_t ptindex = PTE_INDEX(vaddr);
    uint32_t pdindex = PDE_INDEX(vaddr);
    // Get the g_page_directory
    pde_t   *v_pde = (pde_t *)P2V(g_page_directory);
    /*
    debug_t  v_pde_0 = { .directory = v_pde[0] };
    debug_t  v_pde_768 = { .directory = v_pde[768] };
    debug_t  v_pde_769 = { .directory = v_pde[769] };
    printk("v_pde: %p\nvalue hold at 0: 0x%x, at 768: 0x%x, at 769: 0x%x\n",
           v_pde,
           v_pde_0.representation,
           v_pde_768.representation,
           v_pde_769.representation);

           */
    pte_t   *v_pte = NULL;
    if (flags & SP_KERNEL) {
        v_pte = (pte_t *)g_page_table_kernel_space;
    } else if (flags & SP_USER) {
        v_pte = (pte_t *)g_page_table_user_space;
    }
    if (v_pte[ptindex].present == 0) {
        if (flags & SP_USER) v_pte[ptindex].user = 1;
        v_pte[ptindex].rw = 1;
        v_pte[ptindex].address = get_address_value(frame);
        v_pte[ptindex].present = 1;
    } else {
        return NULL;
    }

    v_pde[pdindex].rw = 1;
    v_pde[pdindex].address = get_address_value((void *)V2P(v_pte));
    v_pde[pdindex].present = 1;
    /*
    v_pde_768.directory = v_pde[768];
    v_pde_769.directory = v_pde[pdindex];
    printk("v_pde: %p\nvalue hold at 0: 0x%x, at 768: 0x%x, at 769: 0x%x\n",
           v_pde,
           v_pde_0.representation,
           v_pde_768.representation,
           v_pde_769.representation);
           */
    set_cr3((uint32_t)g_page_directory);
    return (void *)vaddr;
}

void init_paging(uint32_t base_addrs, uint32_t size)
{
    printk("Memory available: %x B\n", size);
    init_pfn_db(size);
    init_frame_page(base_addrs, size);
}
