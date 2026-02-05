const std = @import("std");

pub fn build(b: *std.Build) void {
    const optimize = b.standardOptimizeOption(.{});
    const target = b.resolveTargetQuery(.{
        .cpu_arch = .x86,
        .os_tag = .freestanding,
        .abi = .none,
    });

    // Assembly file to compile with NASM
    const nasm_cmd = b.addSystemCommand(&.{ "nasm", "-f", "elf32" });

    // Add input file
    nasm_cmd.addFileArg(b.path("src/asm/boot.asm")); 
    
    // Add output flag "-o" followed by the file name. 
    // 'boot_obj' is now a "LazyPath" representing the future file.
    nasm_cmd.addArg("-o");
    const boot_obj = nasm_cmd.addOutputFileArg("boot.o");


    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .root_source_file = b.path("src/kernel.zig"),
        .link_libc = false,
        .single_threaded = true,
        // .no_builtin = true, // Removed: usually handled by createModule defaults for freestanding, but keep if strictly needed
    });

    // 5. Create the Executable
    const exe = b.addExecutable(.{
        .name = "kernel.bin",
        .root_module = root_module,
    });

    // Link the NASM Object
    exe.addObjectFile(boot_obj);
    
    // Set the linker script
    exe.setLinkerScript(b.path("linker.ld"));

    // Disable builtin like memcpy and other in zig.
    exe.bundle_compiler_rt = false;

    // Use your custom entry instead of the default one.
    exe.entry = .disabled;

    // Install the Artifact
    b.installArtifact(exe);

    const build_iso_step = b.step("iso", "build the iso for the kernel.\n");
    const input_path = b.path("zig-out/bin/kernel.bin").getPath(b);
    const output_path = b.path("iso/boot").getPath(b);
    const cp = b.addSystemCommand(&.{ "cp", "-f", input_path, output_path});
    const xorriso = b.addSystemCommand(&.{ "xorriso", "-as", "mkisofs", "-o", 
        "nilbogos.iso", "-b", "boot/grub/i386-pc/eltorito.img", "-c", "boot.catalog", 
        "-no-emul-boot", "-boot-load-size", "4", "-boot-info-table", "iso"});

    build_iso_step.dependOn(b.getInstallStep());
    build_iso_step.dependOn(&cp.step);
    build_iso_step.dependOn(&xorriso.step);

    const clean_step = b.step("clean", "clean project and previously done step\n");
    clean_step.makeFn = struct {
        fn make(_: *std.Build.Step, _: std.Build.Step.MakeOptions) anyerror!void {
            std.fs.cwd().deleteTree(".zig-cache") catch |err| {
                if (err != error.FileNotFound) return err;
            };
            std.fs.cwd().deleteTree("zig-out") catch |err| {
                if (err != error.FileNotFound) return err;
            };
            std.log.info("Cleaned zig-cache & zig-out", .{});
            std.fs.cwd().deleteTree("iso/boot/kernel.bin") catch |err| {
                if (err != error.FileNotFound) return err;
            };
            std.log.info("Cleaned kernel.bin from iso dir", .{});
            std.fs.cwd().deleteTree("nilbogos.iso") catch |err| {
                if (err != error.FileNotFound) return err;
            };
            std.log.info("Cleaned nilbogos.iso", .{});
        }
    }.make;
}
