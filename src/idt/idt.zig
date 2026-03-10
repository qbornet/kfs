const std = @import("std");
const mem = @import("../lib/mem.zig");
const console = @import("../vga/vga.zig");

/// All the ISRs used by the idt.
extern fn isr0() void;
extern fn isr1() void;
extern fn isr2() void;
extern fn isr3() void;
extern fn isr4() void;
extern fn isr5() void;
extern fn isr6() void;
extern fn isr7() void;
extern fn isr8() void;
extern fn isr9() void;
extern fn isr10() void;
extern fn isr11() void;
extern fn isr12() void;
extern fn isr13() void;
extern fn isr14() void;
extern fn isr15() void;
extern fn isr16() void;
extern fn isr17() void;
extern fn isr18() void;
extern fn isr19() void;
extern fn isr20() void;
extern fn isr21() void;

comptime {
    _ = mem;
    _ = @import("./interrupt_handler.zig");
}

const KERNEL_CODE_SEGMENT = 0x08;

/// DPL enum u3 bit.
const DescriptorPrivilegeLevel = enum(u3) {
    Ring_0 = 0, // Highest Privilege
    Ring_1,
    Ring_2,
    Ring_3, // Lowest Privilege
};

/// Gate Type enum for Gate Descriptor.
const GateType = enum(u4) {
    TASK_GATE = 0x5, // Task Gate enum offset value is not used.
    INTERUPT_GATE_16 = 0x6, // Interupt Gate enum for 16 bit.
    TRAP_GATE_16 = 0x7, // Trap Gate enum for 16 bit.
    INTERUPT_GATE = 0xE, // Interupt Gate enum for 32bit.
    TRAP_GATE = 0xF, // Trap Gate enum for 32bit.
};


/// Interupt Decriptor used for IDT descriptor array.
const InteruptDescriptor = packed struct {
    offset_1: u16,
    selector: u16,
    _ignored_0: u8,
    type_gate: GateType, // Gate Type more info on `GateType` enum.
    _ignored_1: u1,
    dpl: DescriptorPrivilegeLevel, // DPL bit
    present: u1,
    offset_2: u16,
};

var g_idts: [255]InteruptDescriptor = undefined;
const Idt = packed struct {
    base: u32,
    size: u16,
};

fn loadIdt() void {
    var idt_ptr = Idt{
        .base = @intFromPtr(&g_idts),
        .size = @intCast((g_idts.len * @sizeOf(InteruptDescriptor)) - 1),
    };

    asm volatile(
        \\ cli
        \\ lidt (%[addr])
        :
        : [addr] "r" (&idt_ptr)
        : .{ .memory =  true }
    );
}

pub fn deleteInterruptDescriptor(index: u8) void {
    if (g_idts[index] == 0) return;
    @memset(&g_idts[index], 0);
}

const OffsetFunction = *const fn () callconv(.c) void;
pub fn createInterruptDescriptor(index: u8, offset: OffsetFunction, selector: u16, attributes: GateType, dpl: DescriptorPrivilegeLevel) void {
    if (g_idts[index].present == 1) {
        console.print("Invalid index '{d}' already used\n", .{index});
        return;
    }
    const offset_value = @intFromPtr(offset);
    const offset_1: u16 = @truncate(offset_value);
    const offset_2: u16 = @truncate(offset_value >> 16);
    g_idts[index] = InteruptDescriptor{
        .dpl = dpl,
        .offset_1 = offset_1,
        .offset_2 = offset_2,
        .type_gate = attributes,
        .selector = selector,
        ._ignored_0 = 0, 
        ._ignored_1 = 0,
        .present = 1,
    };
}

pub fn init() void {
    createInterruptDescriptor(0, isr0, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(1, isr1, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(2, isr2, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(3, isr3, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(4, isr4, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(5, isr5, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(6, isr6, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(7, isr7, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(8, isr8, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(9, isr9, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(10, isr10, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(11, isr11, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(12, isr12, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(13, isr13, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(14, isr14, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(15, isr15, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(16, isr16, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(17, isr17, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(18, isr18, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(19, isr19, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(20, isr20, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
    createInterruptDescriptor(21, isr21, KERNEL_CODE_SEGMENT, .INTERUPT_GATE, .Ring_0);
}
