const std = @import("std");
const mem = @import("../lib/mem.zig");

comptime {
    _ = mem;
}

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

pub fn deleteInteruptDescriptor(index: u8) void {
    if (g_idts[index] == 0) return;
    @memset(&g_idts[index], 0);
}

pub fn createInteruptDescriptor(index: u8, offset: u32, selector: u16, attributes: GateType, dpl: DescriptorPrivilegeLevel) void {
    if (g_idts[index] != 0) @panic("index not available");
    const offset_1 = (offset & 0xffff);
    const offset_2 = ((offset >> 16) & 0xffff);
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
    createInteruptDescriptor(0, 0, 0x08, .INTERUPT_GATE, .Ring_0);
}
