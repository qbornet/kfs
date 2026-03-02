const std = @import("std");

pub fn build(b: *std.Build) void {
    const optimize = b.standardOptimizeOption(.{ .preferred_optimize_mode = .ReleaseSmall });
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


    // Unit testing step
    const test_step = b.step("test", "Run unit tests");
    const unit_test = b.addTest(.{
        .root_module = b.createModule(.{
            .root_source_file = b.path("src/root_test.zig"), // this does all the unit test for important function inside the kernel.
            .target = b.resolveTargetQuery(.{
                .cpu_arch = .x86,
                .abi = .none,
                // .os_tag = .freestanding, freestanding not used for testing purpose for function
            }),
            .optimize = optimize,
            .link_libc = false,
            .strip = false, // specify this to have debug info
        }),
    });

    // Run test unit.
    const run_unit_tests = b.addRunArtifact(unit_test);
    test_step.dependOn(&run_unit_tests.step);

    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .root_source_file = b.path("src/kernel.zig"),
        .link_libc = false,
        .single_threaded = true,
        .strip = false, // specify this to have debug info
        // .code_model = .kernel,
        // .no_builtin = true, // Removed: usually handled by createModule defaults for freestanding, but keep if strictly needed
    });

    // Create the Executable
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

    // Use custom entry instead of the default one.
    exe.entry = .disabled;

    // Install the Artifact
    b.installArtifact(exe);

    // Build bootable iso file.
    const build_iso_step = b.step("iso", "build the iso for the kernel.\n");
    const cp = b.addSystemCommand(&.{ "cp", "-f"});
    const kernel_artifact = exe.getEmittedBin();
    cp.addFileArg(kernel_artifact);
    cp.addArg("iso/boot/kernel.bin");
    const xorriso = b.addSystemCommand(&.{ "xorriso", "-as", "mkisofs", "-o", 
        "nilbogos.iso", "-b", "boot/grub/i386-pc/eltorito.img", "-c", "boot.catalog", 
        "-no-emul-boot", "-boot-load-size", "4", "-boot-info-table", "iso"});

    // Need the install step to be done first before doing cp command.
    cp.step.dependOn(b.getInstallStep());

    // Then do cp command first before doing xorriso.
    xorriso.step.dependOn(&cp.step);

    // Finally do xorriso.
    build_iso_step.dependOn(&xorriso.step);

    // Run qemu normal mode
    const run_step = b.step("run", "run the iso file present.\n");

    const qemu = b.addSystemCommand(&.{"qemu-system-i386"});
    qemu.addArg("-cdrom");
    qemu.addFileArg(b.path("nilbogos.iso"));
    qemu.addArgs(&.{"-serial", "mon:stdio"});
    qemu.step.dependOn(build_iso_step);
    run_step.dependOn(&qemu.step);

    // Clean Artifact, cache and iso file.
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
