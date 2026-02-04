#ifndef TEST_PAGING_H
#define TEST_PAGING_H
#define ASSERT(condition, message)      \
    if ((condition)) {                  \
        printk("[PASS] %s\n", message); \
    } else {                            \
        printk("[FAIL] %s\n", message); \
        return;                         \
    }
#include <lib/io.h>
#include <lib/kmalloc.h>
#include <lib/string.h>
#include <paging/pfn.h>
void test_kmalloc(void);
void test_kmalloc_user(void);
void test_kmalloc_invalid_user(void);
#endif
