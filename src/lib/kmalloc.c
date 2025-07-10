#include "kmalloc.h"

extern uintptr_t g_mem_ptr;
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
    g_mem_ptr = (g_mem_ptr + 7) & ~7; // Aligned to uint8_t (byte aligned)

    ptr = (void *)g_mem_ptr;
    g_mem_ptr += size;
    if (g_mem_ptr >= g_mem_end_ptr) {
        printk("Error out of kernel memory\n");
        return NULL;
    }
    g_free_mem_ptr += (size + 7) & ~7;
    g_malloc_count++;
    return ptr;
}

void init_malloc_ptr(void)
{
    g_malloc_count = 0;
    g_mem_ptr = (uintptr_t)&g_mem_ptr + HEAP_OFFSET_START;
    g_free_mem_ptr = g_mem_ptr;
    g_mem_end_ptr = g_mem_ptr + HEAP_SIZE;
    printk("start: %lX, end: %lX, free_mem_ptr: %lX\n",
           g_mem_ptr,
           g_mem_end_ptr,
           g_free_mem_ptr);
    char *old = NULL;
    char *p = (char *)malloc(10);

    printk("start: %lX, end: %lX, free_mem_ptr: %lX\n",
           g_mem_ptr,
           g_mem_end_ptr,
           g_free_mem_ptr);
    printk("[%ld]: Malloc pointer returned: %p\n", g_malloc_count, p);

    strcpy(p, "Hello, Worl");
    printk("p contains: %s\n", p);

    old = p;
    p = (char *)malloc(10);
    printk("start: %lX, end: %lX, free_mem_ptr: %lX\n",
           g_mem_ptr,
           g_mem_end_ptr,
           g_free_mem_ptr);
    printk("[%ld]: Malloc pointer returned: %p\n", g_malloc_count, p);

    strcpy(p, "totoismage");
    printk("p contains: %s, old contains: %s\n", p, old);
    p = (char *)malloc(1000);
    printk("start: %lX, end: %lX, free_mem_ptr: %lX\n",
           g_mem_ptr,
           g_mem_end_ptr,
           g_free_mem_ptr);
}
