const std = @import("std");
const console = @import("vga/vga.zig");
const MULTIBOOT2_BOOTLOADER_MAGIC = 0x36d76289;

export fn kernel_main(magic: u32, mbi_address: u32) void {
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) return;
    if ((mbi_address & 7) == 1) return;

    console.init();
    console.print("Hello {s}\n", .{"World!"});

    while (true) {}
}
