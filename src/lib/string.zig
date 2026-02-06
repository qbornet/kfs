pub fn strlen(src: [*]u8) usize {
    var i: usize = 0;
    while (src[i] != 0) : (i += 1) {}
    return i;
}
