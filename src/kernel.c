#include "kernel.h"

void kernel_main(void)
{
    init_gdt();
    terminal_initialize();

    // identity-map the low mem_end bytes of physical memory
    extern uint32_t g_end; // set by linker at end of .bss
    uint32_t        mem_end = (uint32_t)&g_end + 0x00A00000;

    mmu_init(mem_end);

    printk("=== MMU self-test ===\n");

    int r;
    r = test_identity_map();
    printk("identity_map:   %s\n", r == 0 ? "OK" : "FAIL");
    r = test_map_unmap();
    printk("map_unmap:      %s\n", r == 0 ? "OK" : "FAIL");
    r = test_access_dirty_bits();
    printk("access_dirty:   %s\n", r == 0 ? "OK" : "FAIL");
    r = test_wsclock_eviction();
    printk("wsclock_evict:  %s\n", r == 0 ? "OK" : "FAIL");
    r = test_pte_uniqueness();
    printk("pte_uniqueness: %s\n", r == 0 ? "OK" : "FAIL");
    r = test_remap_overwrite();
    printk("remap_overwrite: %s\n", r == 0 ? "OK" : "FAIL");
    r = test_switch_directory_isolation();
    printk("dir_isolation:  %s\n", r == 0 ? "OK" : "FAIL");
    r = test_kmalloc_basic();
    printk("kmalloc_basic:  %s\n", r == 0 ? "OK" : "FAIL");
    r = test_kmalloc_aligned();
    printk("kmalloc_aligned:%s\n", r == 0 ? "OK" : "FAIL");
    r = test_kmalloc_zero();
    printk("kmalloc_zero:   %s\n", r == 0 ? "OK" : "FAIL");
    r = test_kmalloc_multiple();
    printk("kmalloc_multi:  %s\n", r == 0 ? "OK" : "FAIL");

    printk("=== tests complete ===\n");

    for(;;) {
        asm volatile("hlt");
    }
}
