const string = @import("string.zig");

pub fn memcpy(dst: [*]u8, src: [*]u8, size: usize) [*]u8 {
    var i: usize = 0;
    if (size != string.strlen(dst)) return dst;
    while (i < size) : (i += 1) dst[i] = src[i];
}
pub fn memset(dst: [*]u8, byte: u8, size: usize) [*]u8 {
    var i: usize = 0;
    while (i < size) : (i += 1) dst[i] = byte;
    return dst;
}
