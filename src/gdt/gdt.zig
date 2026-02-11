const std = @import("std");

const GdtFlags = enum(u32) {
    System =        1 << 12, // 12 bit system descriptor
    Present =       1 << 15, // 15 bit present descriptor (if set in memory)
    Accessed =      1 << 20, // 20 bit available for use by system
    DefaultBig =    1 << 22, // 22 bit default operation size (if set 32-bit segment)
    Granularity =   1 << 23, // 23 bit granularity flag
};

const DescriptorTypes = enum(u32) {
    Accessed =      1 << 8, // 8 bit access bit for segment
    Readable =      1 << 9, // 9 bit write bit for segment
    Expend =        1 << 10, // 10 bit expend for data segment (expand down grow stack down)
    Data =          1 << 11, // 11 bit data segment (if set to 0 data) else code
};

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

var g_sdes: [16]SegmentDescriptor = &.{};

fn createDescriptorPrivilege(lvl: DescriptorPrivilegeLevel) u32 {
    return lvl << 13;
}

fn createSegmentDescriptor(
    base:u32,
    limit:u32,
    flags: GdtFlags,
    types: DescriptorTypes,
    lvl: DescriptorPrivilegeLevel,
    segs: *SegmentDescriptor
) SegmentDescriptor {
    segs.start = (limit & 0xffff) | (base & 0xffff) << 16;
    limit >>= 16;
    base >>= 16;

    const low_address: u32 = (base & 0xff);
    base >>= 8;

    const high_address: u32 = (base & 0xff) << 24;
    base >>= 8;

    const limit_value: u32 = (limit&0xf) << 16;

    segs.end = low_address | high_address | limit_value | types | flags | createDescriptorPrivilege(lvl);
}

inline fn loadGdt() void {
    const gdt = packed struct {
        size: u16,
        addrs: u32
    };
    gdt.size = @sizeOf(g_sdes);
    gdt.addrs = @intFromPtr(g_sdes);
    asm volatile ("lgdt %[addr]\n\t" :: [addr] "r" (gdt));
}

pub fn initGdt() void {
    // NULL DESCRIPTOR
    createSegmentDescriptor(0, 0, 0, 0, 0, &g_sdes[0]);

    // KERNEL CODE DESCRIPTOR
    createSegmentDescriptor(
        0x00000000,
        0xFFFFFFFF,
        .{ .Granularity, .Present, .System, .DefaultBig },
        .{ .Data, .Readable },
        .Ring_0,
        &g_sdes[1]
    );

    // KERNEL DATA DESCRIPTOR
    createSegmentDescriptor(
        0x00000000,
        0xFFFFFFFF,
        .{ .Granularity, .System, .Present, .DefaultBig, },
        .{ .Readable },
        .Ring_0,
        &g_sdes[2]
    );

    // KERNEL STACK DESCRIPTOR
    createSegmentDescriptor(
        0x00000000,
        0xFFFFFFFF,
        .{ .Granularity, .System, .Present, .DefaultBig },
        .{ .Readable, .Expend },
        .Ring_0,
        &g_sdes[3]
    );
    loadGdt();
}
