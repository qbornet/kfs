{
  description = "Flake for KFS";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
  flake-utils.lib.eachSystem[ "x86_64-linux" "aarch64-linux" ] (system:
    let
        pkgs = import nixpkgs { inherit system; };
        crossPkgs = pkgs.pkgsCross.gnu32;
    in {
        devShells.default = crossPkgs.mkShell {
            # Dependencies
            nativeBuildInputs = with pkgs; [
                # Building and compile (compiler pre-built in crossPkgs.mkShell)
                gnumake
                bison
                flex
                nasm

                ## FOR C
                # Linter & Formater
                #clang-tools

                # Generate compile_commands.json
                #bear

                # Xorriso & Bootloader
                xorriso
                grub2

                # Emulation
                qemu

                # debug tools
                gdb
                socat

                # Compiler & Linter (zig)
                zig
                zig-zlint
            ];
            buildInputs = [];

            shellHook = ''
                echo "------------------------------------------------------------------"
                echo "🌲    Kernel Dev Environment Loaded"
                echo "🎯    Target: i686 (32-bit x86)"
                echo "🛠     Compiler (zig): $( zig version )"
                echo "     Shell: $( basename $SHELL )"
                echo "------------------------------------------------------------------"
                export ZIG_GLOBAL_CACHE_DIR=$HOME/.cache/zig
            '';
            NIXOS_DEV_ENV="ON";
        };
    }
  );
}
