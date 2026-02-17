const std = @import("std");
const console = @import("vga/vga.zig");
const gdt = @import("gdt/gdt.zig");
const MULTIBOOT2_BOOTLOADER_MAGIC = 0x36d76289;

export fn kernel_main(magic: u32, mbi_address: u32) void {
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) return;
    if ((mbi_address & 7) == 1) return;

    console.init();
    console.print("console init done !\n", .{});
    gdt.init();
    console.print("gdt init done !\n", .{});
    while (true) {
        asm volatile ("hlt");
    }
}
