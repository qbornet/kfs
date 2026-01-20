#include <lib/kmalloc.h>

extern uint32_t *g_kernel_end;
static uintptr_t g_mem_ptr;
static uintptr_t g_mem_end_ptr;
static uint32_t  g_malloc_count;
uintptr_t        g_free_mem_ptr;

static void     *malloc(size_t size)
{
    void *ptr;

    if (size > PAGE_SIZE) {
        // Return null for the moment should use other alloc for page and above.
        return NULL;
    }
    if (g_mem_ptr >= g_mem_end_ptr) {
        printk("Error out of kernel memory\n");
        return NULL;
    }
    if (!g_mem_ptr) g_mem_ptr = g_free_mem_ptr;
    g_mem_ptr = (g_mem_ptr + 3) & ~3; // Alignment for 4-byte.

    ptr = (void *)g_mem_ptr;
    g_mem_ptr += size;
    if (g_mem_ptr >= g_mem_end_ptr) {
        printk("Error out of kernel memory\n");
        return NULL;
    }
    g_free_mem_ptr += (size + 3) & ~3; // Alignment for 4-byte
    g_malloc_count++;
    return ptr;
}

void init_malloc_ptr(void)
{
    g_mem_ptr = (uint32_t)g_kernel_end;
    uint32_t mem_aligned = (g_mem_ptr + 3) & ~3; // Alignment for 4-byte.
    g_malloc_count = 0;
    g_mem_ptr = (uintptr_t)&mem_aligned + HEAP_OFFSET_START;
    g_free_mem_ptr = g_mem_ptr;
    g_mem_end_ptr = g_mem_ptr + HEAP_SIZE;
    printk("start: %X, end: %X, free_mem_ptr: %X\n",
           g_mem_ptr,
           g_mem_end_ptr,
           g_free_mem_ptr);
    char *old = NULL;
    char *p = (char *)malloc(10);

    printk("start: %X, end: %X, free_mem_ptr: %X\n",
           g_mem_ptr,
           g_mem_end_ptr,
           g_free_mem_ptr);
    printk("[%d]: Malloc pointer returned: %p\n", g_malloc_count, p);

    strcpy(p, "Hello, Worl");
    printk("p contains: %s\n", p);

    old = p;
    p = (char *)malloc(10);
    printk("start: %X, end: %X, free_mem_ptr: %X\n",
           g_mem_ptr,
           g_mem_end_ptr,
           g_free_mem_ptr);
    printk("[%d]: Malloc pointer returned: %p\n", g_malloc_count, p);

    strcpy(p, "totoismage");
    printk("p contains: %s, old contains: %s\n", p, old);
    p = (char *)malloc(1000);
    printk("start: %X, end: %X, free_mem_ptr: %X\n",
           g_mem_ptr,
           g_mem_end_ptr,
           g_free_mem_ptr);
}
