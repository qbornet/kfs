#include <gdt.h>
#include <paging/test_paging.h>

static void test_kmalloc_kernel(void)
{
    printk("\n=== Starting Unit Test: KMALLOC ===\n");

    /* * TEST 1: Basic Allocation & Size Rounding
     * Request 10 bytes. Allocator should give at least 1 Page (4096).
     */
    void *ptr1 = kmalloc(10, SP_KERNEL);
    ASSERT(ptr1 != NULL, "Allocated 10 bytes");
    ASSERT(ksize(ptr1) == 1, "Size rounded to 1 page (4KB)");

    /* * TEST 2: Page Boundary Crossing
     * Request 4097 bytes (1 Page + 1 Byte). Allocator should give 2 Pages
     * (8192).
     */
    void *ptr2 = kmalloc(4097, SP_KERNEL);
    ASSERT(ptr2 != NULL, "Allocated 4097 bytes");
    ASSERT(ksize(ptr2) == 2, "Size rounded to 2 pages (8KB)");
    ASSERT(ptr1 != ptr2, "Pointers are unique");

    /* * TEST 3: Data Integrity (Read/Write)
     * Write a pattern to the entire allocated block and verify it sticks.
     */
    char *data = (char *)ptr2;
    // Write pattern to the start
    memcpy(data, "START", 5);
    // Write pattern to the very end (2nd page) to ensure full mapping
    memcpy(data + 8180, "END!", 4);

    ASSERT(data[0] == 'S' && data[1] == 'T',
           "Write to start of block verified");
    ASSERT(data[8180] == 'E' && data[8183] == '!',
           "Write to end of block verified");

    /* * TEST 4: Zero Allocation Edge Case
     * Allocating 0 bytes should either return NULL or a minimum block.
     * Adjust expectation based on your specific implementation.
     */
    void *ptr3 = kmalloc(0, SP_KERNEL);
    if (ptr3 == NULL) {
        printk("[INFO] kmalloc(0) returned NULL (Valid behavior)\n");
    } else {
        printk("[INFO] kmalloc(0) returned pointer (Valid behavior)\n");
        kfree(ptr3);
    }

    /* * TEST 5: Freeing
     * We free ptr1 and check if we crash (basic check).
     * Advanced: allocate again and see if we get the same address (Recycling).
     */
    kfree(ptr1);
    printk("[PASS] kfree(ptr1) executed without crash\n");

    void *ptr1_new = kmalloc(10, SP_KERNEL);
    if (ptr1_new == ptr1) {
        printk("[INFO] Allocator recycled freed memory address\n");
    } else {
        printk("[INFO] Allocator returned fresh address\n");
    }

    // Cleanup rest
    kfree(ptr2);
    kfree(ptr1_new);

    printk("=== End Unit Test: KMALLOC ===\n\n");
}

static void test_kmalloc_invalid(uint8_t test)
{
    if (test == 0) return;
    printk("\n=== Starting Unit Test: KMALLOC (PAGE FAULT) ===\n");
    /* * TEST 1
     *  Take non map virtual address and try to access it.
     */
    if (test == 1) {
        void *vaddr = (void *)0x100000;
        memcpy(vaddr, "titi", 4);
        ASSERT(ksize(vaddr) == 0, "Not a valid address\n");
        printk(
            "[PASS] Writing a invalid address (no crash) page fault handle\n");
    }

    /* * TEST 2
     *  Take map virtual address free it and try to access it.
     */
    if (test == 2) {
        void *vaddr = kmalloc(10, SP_KERNEL);
        kfree(vaddr);
        memcpy(vaddr, "toto", 4);
        ASSERT(ksize(vaddr) == 0,
               "Not free pointer not a valid address anymore\n");
        printk(
            "[PASS] Writing invalid address after free (no crash) page fault "
            "handle\n");
    }

    printk("=== End Unit Test: KMALLOC INVALID ===\n\n");
}

void test_kmalloc_user(void)
{
    printk("\n=== Starting Unit Test: KMALLOC (USER) ===\n");

    /* * TEST 1: Basic Allocation & Size Rounding
     * Request 10 bytes. Allocator should give at least 1 Page (4096).
     */
    void *ptr1 = kmalloc(10, SP_USER);
    ASSERT(ptr1 != NULL, "Allocated 10 bytes");
    ASSERT(ksize(ptr1) == 1, "Size rounded to 1 page (4KB)");

    /* * TEST 2: Page Boundary Crossing
     * Request 4097 bytes (1 Page + 1 Byte). Allocator should give 2 Pages
     * (8192).
     */
    void *ptr2 = kmalloc(4097, SP_USER);
    ASSERT(ptr2 != NULL, "Allocated 4097 bytes");
    ASSERT(ksize(ptr2) == 2, "Size rounded to 2 pages (8KB)");
    ASSERT(ptr1 != ptr2, "Pointers are unique");

    /* * TEST 3: Data Integrity (Read/Write)
     * Write a pattern to the entire allocated block and verify it sticks.
     */
    char *data = (char *)ptr2;
    // Write pattern to the start
    memcpy(data, "START", 5);
    // Write pattern to the very end (2nd page) to ensure full mapping
    memcpy(data + 8180, "END!", 4);

    ASSERT(data[0] == 'S' && data[1] == 'T',
           "Write to start of block verified");
    ASSERT(data[8180] == 'E' && data[8183] == '!',
           "Write to end of block verified");

    /* * TEST 4: Zero Allocation Edge Case
     * Allocating 0 bytes should either return NULL or a minimum block.
     * Adjust expectation based on your specific implementation.
     */
    void *ptr3 = kmalloc(0, SP_USER);
    if (ptr3 == NULL) {
        printk("[INFO] kmalloc(0) returned NULL (Valid behavior)\n");
    } else {
        printk("[INFO] kmalloc(0) returned pointer (Valid behavior)\n");
        kfree(ptr3);
    }

    /* * TEST 5: Freeing
     * We free ptr1 and check if we crash (basic check).
     * Advanced: allocate again and see if we get the same address (Recycling).
     */
    kfree(ptr1);
    printk("[PASS] kfree(ptr1) executed without crash\n");

    void *ptr1_new = kmalloc(10, SP_USER);
    if (ptr1_new == ptr1) {
        printk("[INFO] Allocator recycled freed memory address\n");
    } else {
        printk("[INFO] Allocator returned fresh address\n");
    }

    // Cleanup rest
    kfree(ptr2);
    kfree(ptr1_new);

    printk("=== End Unit Test: KMALLOC (USER) ===\n\n");
}

void test_kmalloc_invalid_user(void)
{
    printk("\n=== Starting Unit Test: KMALLOC PAGE FAULT (USER) ===\n");
    void *vaddr = kmalloc(10, SP_KERNEL);
    memcpy(vaddr, "tata", 4);
    ASSERT(vaddr == NULL,
           "If CPL > 0 should not be able to return a pointer\n");
    ASSERT(ksize(vaddr) == 0,
           "Should not be able to create a SP_KERNEL if user\n");

    vaddr = (void *)0xC02FF000;
    memcpy(vaddr, "tito", 4);
    printk("Should page fault\n");
    printk("=== End Unit Test: KMALLOC PAGE FAULT (USER) ===\n\n");
}

void test_kmalloc(void)
{
    test_kmalloc_kernel();
    test_kmalloc_invalid(0);
    printk("before jumping to usermode\n");
    jump_usermode();
}
