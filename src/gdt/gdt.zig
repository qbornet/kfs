const std = @import("std");
const console = @import("../vga/vga.zig");
const mem = @import("../lib/mem.zig");

comptime {
    _ = mem;
}

/// GdtFlags contains all the flags that are not Types or DPL for segment descriptors.
const GdtFlags = enum(u32) {
    System =        1 << 12, // 12 bit system descriptor
    Present =       1 << 15, // 15 bit present descriptor (if set in memory)
    Accessed =      1 << 20, // 20 bit available for use by system
    DefaultBig =    1 << 22, // 22 bit default operation size (if set 32-bit segment)
    Granularity =   1 << 23, // 23 bit granularity flag
};

/// Decsriptores Types for the 8-11 bit inside segment descriptor.
const DescriptorTypes = enum(u32) {
    Accessed =      1 << 8, // 8 bit access bit for segment
    Readable =      1 << 9, // 9 bit write bit for segment
    Expend =        1 << 10, // 10 bit expend for data segment (expand down grow stack down)
    Data =          1 << 11, // 11 bit data segment (if set to 0 data) else code
};

/// For Segment Descriptor privilege level needed to be used.
const DescriptorPrivilegeLevel = enum(u3) {
    Ring_0 = 0,
    Ring_1,
    Ring_2,
    Ring_3,
};

const SegmentDescriptor = packed struct {
    start: u32,
    end: u32,
};

var g_sdes: [16]SegmentDescriptor align(8) = [_]SegmentDescriptor{ .{.start =  0, .end = 0} } ** 16;

fn createDescriptorPrivilege(lvl: DescriptorPrivilegeLevel) u32 {
    const lvl_int: u32 = @intFromEnum(lvl);
    return lvl_int << 13;
}

fn createSegmentDescriptor(
    base:u32,
    limit:u32,
    flags: u32,
    types: u32,
    lvl: DescriptorPrivilegeLevel,
    segs: *SegmentDescriptor
) void {
    segs.start = (limit & 0xffff) | (base & 0xffff) << 16;
    var new_limit = limit >> 16;
    var new_base = base >> 16;

    const low_address: u32 = (new_base & 0xff);
    new_base >>= 8;

    const high_address: u32 = (new_base & 0xff) << 24;
    new_base >>= 8;

    const limit_value: u32 = (new_limit&0xf) << 16;
    new_limit >>= 16;

    segs.end = low_address | high_address | limit_value | types | flags | createDescriptorPrivilege(lvl);
}

const Gdt = packed struct {
    size: u16,
    addrs: u32,
};

inline fn loadGdt() void {
    var gdt: Gdt = .{ .size = 0, .addrs = 0 };
    gdt.size = g_sdes.len - 1;
    gdt.addrs = @intFromPtr(&g_sdes);
    asm volatile ("lgdt %[addr]\n\t" :: [addr] "m" (gdt));
}

pub fn init() void {
    // NULL DECSRIPTOR
    for (0..16) |i| {
        console.print("[{d}]g_sdes: 0x{x}\n", .{i,@intFromPtr(&g_sdes[i])});
    }

    while (true) {
        asm volatile ("hlt");
    }
    var flags: u32 = @intFromEnum(GdtFlags.Granularity) | @intFromEnum(GdtFlags.Present) | @intFromEnum(GdtFlags.System) | @intFromEnum(GdtFlags.DefaultBig);
    var types: u32 = @intFromEnum(DescriptorTypes.Data) | @intFromEnum(DescriptorTypes.Readable);
    // KERNEL CODE DESCRIPTOR
    createSegmentDescriptor(
        0x00000000,
        0xFFFFFFFF,
        flags,
        types,
        .Ring_0,
        &g_sdes[1]
    );

    flags = @intFromEnum(GdtFlags.Granularity) | @intFromEnum(GdtFlags.Present) | @intFromEnum(GdtFlags.System) | @intFromEnum(GdtFlags.DefaultBig);
    types = @intFromEnum(DescriptorTypes.Readable);
    // KERNEL DATA DESCRIPTOR
    createSegmentDescriptor(
        0x00000000,
        0xFFFFFFFF,
        flags,
        types,
        .Ring_0,
        &g_sdes[2]
    );

    flags = @intFromEnum(GdtFlags.Granularity) | @intFromEnum(GdtFlags.System) | @intFromEnum(GdtFlags.Present) | @intFromEnum(GdtFlags.DefaultBig);
    types = @intFromEnum(DescriptorTypes.Readable) | @intFromEnum(DescriptorTypes.Expend);
    // KERNEL STACK DESCRIPTOR
    createSegmentDescriptor(
        0x00000000,
        0xFFFFFFFF,
        flags,
        types,
        .Ring_0,
        &g_sdes[3]
    );
    var i: usize = 0;
    while (i < 3) : (i += 1) {
        console.print("[{d}]: {*}\n", .{i, &g_sdes[i]});
    }
    // loadGdt();
}
