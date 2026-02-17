const std = @import("std");
const builtin = @import("builtin");
const assert = std.debug.assert;

comptime {
    if (builtin.object_format != .c) {
        @export(&memset, .{ .name = "memset", .linkage = .weak, .visibility = .default });
        @export(&__memset, .{ .name = "__memset", .linkage = .weak, .visibility = .default });
        @export(&memcpy, .{ .name = "memcpy", .linkage = .weak, .visibility = .default });
        @export(&memmove, .{ .name = "memmove", .linkage = .weak, .visibility = .default });
    }
}

fn memmove(opt_dest: ?[*]u8, opt_src: ?[*]const u8, len: usize) callconv(.c) ?[*]u8 {
    const dest = opt_dest.?;
    const src = opt_src.?;

    if (@intFromPtr(dest) < @intFromPtr(src)) {
        for (0..len) |i| {
            dest[i] = src[i];
        }
    } else {
        for (0..len) |i| {
            dest[len - 1 - i] = src[len - 1 - i];
        }
    }
    return dest;
}

fn memcpy(noalias dest: ?[*]u8, noalias src: ?[*]const u8, len: usize) callconv(.c) ?[*]u8 {
    @setRuntimeSafety(false);

    for (0..len) |i| {
        dest.?[i] = src.?[i];
    }

    return dest;
}

fn __memset(dest: ?[*]u8, c: u8, n: usize, dest_n: usize) callconv(.c) ?[*]u8 {
    if (dest_n < n)
        @panic("buffer overflow");
    return memset(dest, c, n);
}

fn memset(dest: ?[*]u8, c: u8, len: usize) callconv(.c) ?[*]u8 {
    @setRuntimeSafety(false);

    if (len != 0) {
        var d = dest.?;
        var n = len;
        while (true) {
            d[0] = c;
            n -= 1;
            if (n == 0) break;
            d += 1;
        }
    }
    return dest;
}
