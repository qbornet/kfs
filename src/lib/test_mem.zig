const std = @import("std");
const mem = @import("mem.zig");
const testing = std.testing;

comptime {
    _ = mem;
}

// ---------------
// TESTING MEMMOVE
// ---------------

test "memmove - non-overlapping" {
    var dest = [_]u8{ 0, 0, 0, 0, 0 };
    const src = [_]u8{ 1, 2, 3, 4, 5 };

    _ = mem.memmove(&dest, &src, 5);
    try testing.expectEqualSlices(u8, &src, &dest);
}

test "memmove - overlapping dest > src (shift right)" {
    var buf = [_]u8{ 1, 2, 3, 4, 5, 0, 0 };
    
    // Copy {1, 2, 3, 4, 5} to start at index 2 -> {1, 2, 1, 2, 3, 4, 5}
    _ = mem.memmove(buf[2..].ptr, buf[0..].ptr, 5);
    
    const expected = [_]u8{ 1, 2, 1, 2, 3, 4, 5 };
    try testing.expectEqualSlices(u8, &expected, &buf);
}

test "memmove - overlapping dest < src (shift left)" {
    var buf = [_]u8{ 0, 0, 1, 2, 3, 4, 5 };
    
    // Copy {1, 2, 3, 4, 5} to start at index 0 -> {1, 2, 3, 4, 5, 4, 5}
    _ = mem.memmove(buf[0..].ptr, buf[2..].ptr, 5);
    
    const expected = [_]u8{ 1, 2, 3, 4, 5, 4, 5 };
    try testing.expectEqualSlices(u8, &expected, &buf);
}

test "default @builtin for memmove" {
    var dest1 = [_]u8{ 0, 0, 0, 0, 0 };
    const src1 = [_]u8{ 1, 2, 3, 4, 5 };
    @memmove(&dest1, &src1);

    var dest2 = [_]u8{ 0, 0, 0, 0, 0 };
    const src2 = [_]u8{ 1, 2, 3, 4, 5 };
    _ = mem.memmove(&dest2, &src2, 5);
    try testing.expectEqualSlices(u8, &dest1, &dest2);
}

test "memcpy - basic copy" {
    var dest: [10]u8 = undefined;
    const src = "Hello Zig!".*;

    _ = mem.memcpy(&dest, &src, src.len);
    
    try testing.expectEqualSlices(u8, &src, &dest);
}

// --------------
// TESTING MEMCPY
// --------------

test "memcpy - small buffer" {
    var dest = [_]u8{ 0, 0, 0 };
    const src = [_]u8{ 42, 43, 44 };

    _ = mem.memcpy(&dest, &src, 3);
    
    try testing.expectEqualSlices(u8, &src, &dest);
}

test "memcpy - zero length" {
    var dest = [_]u8{ 1, 1, 1 };
    const src = [_]u8{ 2, 2, 2 };

    _ = mem.memcpy(&dest, &src, 0);

    try testing.expectEqual(dest[0], 1);
}

test "default @builtin for memcpy" {
    var dest1: [5]u8 = undefined;
    const src = [_]u8{ 10, 20, 30, 40, 50 };

    @memcpy(dest1[0..], src[0..]);
    var dest2: [5]u8 = undefined;
    _ = mem.memcpy(&dest2, &src, 5);
    try testing.expectEqualSlices(u8, &dest1, &dest2);
}

// --------------
// TESTING MEMSET
// --------------

test "memset - basic fill" {
    var buf = [_]u8{ 1, 1, 1, 1, 1 };
    
    // Fill with 0
    _ = mem.memset(&buf, 0, 5);
    try testing.expectEqualSlices(u8, &[_]u8{ 0, 0, 0, 0, 0 }, &buf);

    // Fill with a specific byte
    _ = mem.memset(&buf, 0xAA, 3);
    try testing.expectEqualSlices(u8, &[_]u8{ 0xAA, 0xAA, 0xAA, 0, 0 }, &buf);
}

test "memset - zero length" {
    var buf = [_]u8{ 5, 5, 5 };
    _ = mem.memset(&buf, 0, 0);
    
    // Buffer should remain unchanged
    try testing.expectEqualSlices(u8, &[_]u8{ 5, 5, 5 }, &buf);
}

test "__memset - safety check" {
    var buf: [10]u8 = undefined;
    
    // This should work fine (n <= dest_n)
    _ = mem.__memset(&buf, 0xFF, 5, 10);
    try testing.expectEqual(buf[0], 0xFF);
    try testing.expectEqual(buf[4], 0xFF);
}

test "default @builtin for memset" {
    var buf1: [10]u8 = undefined;
    var buf2: [10]u8 = undefined;
    
    @memset(buf1[0..], 0);
    _ = mem.__memset(buf2[0..], 0, 10, 10);
    
    for (buf1) |byte| {
        try testing.expectEqual(byte, 0);
    }
    for (buf2) |byte| {
        try testing.expectEqual(byte, 0);
    }
    try testing.expectEqualSlices(u8, &buf1, &buf2);
}
