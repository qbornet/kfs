const std = @import("std");

const GdtFlags = enum(u32) {
    System =        1 << 12, // 12 bit
    Present =       1 << 15, // 15 bit
    Accessed =      1 << 20, // 20 bit
    DefaultBig =    1 << 22, // 22 bit
    Granularity =   1 << 23, // 23 bit
};

const DescriptorTypes = enum(u4) {
    Accessed =      1 << 1, // 8 bit
    Readable =      1 << 2, // 9 bit
    Conforming =    1 << 3, // 10 bit
    Data =          1 << 4, // 11 bit
};

const PrivilegeLevel = enum(u3) {
    Ring_0 = 0,
    Ring_1,
    Ring_2,
    Ring_3,
};
