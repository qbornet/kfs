const std = @import("std");
const console = @import("../vga/vga.zig");
pub const IsrContext = packed struct {
    // Pushed manually in isr_common_stub (Segments)
    gs: u32, fs: u32, es: u32, ds: u32,

    // Pushed by pushad
    edi: u32,
    esi: u32,
    ebp: u32,
    unused_esp: u32,
    ebx: u32,
    edx: u32,
    ecx: u32,
    eax: u32,

    // Pushed by the ISR macro
    int_no: u32,
    error_code: u32,

    // Pushed by the CPU hardware
    eip: u32,
    cs: u32,
    eflags: u32,
};

// Array of exception messages for better printing
const exception_messages = [_][]const u8{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Faulty x87 Float Unit",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
};

export fn interrupt_handler(ctx: *IsrContext) callconv(.c) void {
    const msg = if (ctx.int_no < exception_messages.len) 
        exception_messages[ctx.int_no] 
    else 
        "Unknown Exception";

    // Assuming your console/vga print works
    console.print("\n[EXCEPTION {d}: {s}]\n", .{ctx.int_no, msg});
    console.print("EIP: 0x{x} | ERR: 0x{x} | EAX: 0x{x}\n", .{ctx.eip, ctx.error_code, ctx.eax});

    // For critical exceptions, we hang
    if (ctx.int_no <= 21) {
        while (true) {}
    }
}
