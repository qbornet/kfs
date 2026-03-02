const std = @import("std");
const mem = @import("../lib/mem.zig");
const serial = @import("../serial/serial.zig");

//  Needed when calling @memset or other exported symbols.
comptime {
    _ = mem;
}

const VGA_WIDTH = 80;
const VGA_HEIGHT = 25;
const VGA_SIZE = VGA_WIDTH * VGA_HEIGHT;

var g_row: usize = 0;
var g_column: usize = 0;
var g_color: Color = .init(.green, .black);
var g_buffer = @as([*]volatile u16, @ptrFromInt(0xB8000));

pub const ColorType = enum(u4) {
    black = 0,
    blue = 1,
    green = 2,
    cyan = 3,
    red = 4,
    magenta = 5,
    brown = 6,
    light_gray = 7,
    dark_gray = 8,
    light_blue = 9,
    light_green = 10,
    light_cyan = 11,
    light_red = 12,
    light_magenta = 13,
    light_brown = 14,
    white = 15,
};

const Color = packed struct(u8) {
    fg: ColorType,
    bg: ColorType,

    pub fn init(fg: ColorType, bg: ColorType) Color {
        return .{ .fg = fg, .bg = bg };
    }

    pub fn getVgaChar(self: Color, char: u8) u16 {
        return @as(u16, @as(u8, @bitCast(self))) << 8 | char;
    }
};

fn checkAndScroll() void {
    if (g_row == VGA_HEIGHT) {
        g_row = 0;
    }
}

pub fn init() void {
    clear();
    serial.init();
}

pub fn setColor(color: Color) void {
    g_color = color;
}

pub fn clear() void {
    @memset(g_buffer[0..VGA_SIZE], Color.getVgaChar(g_color, ' '));
}

pub fn printCharAt(char: u8, color: Color, x: usize, y: usize) void {
    const index = y * VGA_WIDTH + x;
    g_buffer[index] = color.getVgaChar(char);
}

pub fn printChar(char: u8) void {
    switch (char) {
        '\n' => {
            g_column = 0;
            g_row += 1;
            checkAndScroll();
        },
        else => {
            printCharAt(char, g_color, g_column, g_row);
            g_column += 1;
            if (g_column == VGA_WIDTH) {
                g_column = 0;
                g_row += 1;
                checkAndScroll();
            }
        },
    }
}

pub fn printString(str: []const u8) void {
    for (str) |char| {
        printChar(char);
    }
}

/// `splat` is the numbers of time that you will write the last items of data slices (this is a logical memset),
/// this is done so that it's more efficient and avoid indirect calls. (only if last slice is equal to 0 do nothing).
fn drain(w: *std.Io.Writer, data: []const []const u8, splat: usize) !usize {
    std.debug.assert(data.len != 0);

    var consumed: usize = 0;
    const pattern = data[data.len - 1];
    const splat_len = pattern.len * splat;

    // Check if we already have a buffer present if so write and set end to 0.
    if (w.end != 0) {
        const str = w.buffered();
        serial.putstr(str);
        printString(str);
        w.end = 0;
    }

    // Take all the data and print (remove the last slices because of the splat logic).
    for (data[0 .. data.len - 1]) |bytes| {
        serial.putstr(bytes);
        printString(bytes);
        consumed += bytes.len;
    }

    // If last items of data is not 0, write it splat times.
    switch (pattern.len) {
        0 => {},
        else => {
            for (0..splat) |_| {
                serial.putstr(pattern);
                printString(pattern);
            }
        },
    }

    consumed += splat_len;
    return consumed;
}

pub fn writer(buffer: []u8) std.Io.Writer {
    return .{
        .buffer = buffer,
        .end = 0,
        .vtable = &.{
            .drain = drain,
        },
    };
}

pub fn print(comptime fmt: []const u8, args: anytype) void {
    var w = writer(&.{});
    w.print(fmt, args) catch return;
}
