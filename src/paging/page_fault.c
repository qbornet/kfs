#include <paging/page_fault.h>

void page_fault_handler(register_t *regs)
{
    uint32_t fault_addr;
    asm volatile("mov %%cr2, %0"
                 : "=r"(fault_addr));

    int present = !(regs->err_code & 0x1);
    int rw = regs->err_code & 0x2;
    int user = regs->err_code & 0x4;
    int reserved = regs->err_code & 0x8;
    int id = regs->err_code & 0x10;

    printk("\nPAGE FAULT (Interrupt 14)\n");
    printk("----------------------------------\n");
    printk("Faulting Address: 0x%x\n", fault_addr);
    printk("Error Code:       0x%x\n", regs->err_code);

    printk("Reason: ");
    if (present) printk(" [Page Not Present] ");
    if (rw)
        printk(" [Write Violation] ");
    else
        printk(" [Read Violation] ");
    if (user) printk(" [User-Mode] ");
    if (reserved) printk(" [Reserverd Bit Violation] ");
    if (id) printk(" [Instruction Fetch] ");
    printk("\n");

    printk("stop the kernel.\n");
    while (1) asm volatile("cli; hlt");
}
