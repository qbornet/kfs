# KFS (Kernel From Scratch)

![Status](https://img.shields.io/badge/Status-Work_In_Progress-red?style=flat-square)
![Architecture](https://img.shields.io/badge/Architecture-x86_%E2%86%92_x86__64-blue?style=flat-square)
![Target](https://img.shields.io/badge/Target-POSIX_Compliant-success?style=flat-square)
![Language (Main)](https://img.shields.io/badge/Language-C-00599C?style=flat-square&logo=c)
![Language (Rewrite)](https://img.shields.io/badge/Language-Zig_%E2%89%A50.15.0-orange?style=flat-square&logo=zig)

**KFS (Kernel From Scratch)** is an educational and ambitious project to build a custom, Unix-like, POSIX-compliant operating system kernel from the ground up. The only external component used is the bootloader; everything else is written from scratch. 

The primary philosophy of this kernel is to provide a clean x86 implementation with minimal legacy baggage (stripping out useless 16-bit or obsolete x86 features). The ultimate long-term goal of the project is to evolve into a fully-fledged x86_64 kernel.

> **⚠️ Project Status:** This project is currently **unfinished** and in active development. 
> 
> **Note on Languages:** The `main` branch is currently written in **C**. However, a complete rewrite of the kernel is currently underway in **Zig**. You can track the progress of the Zig rewrite on the `zig-kfs` branch.

---

## 🚀 Feature Checklist

Here is the current development status of the kernel's core features:

### ✅ Completed
- [x] **Boot & Initialization:** Hardware information gathering during boot.
- [x] **Memory Management:** Virtual Memory and Paging implemented.
- [x] **Higher Half Kernel:** Kernel mapped to the higher half of memory.
- [x] **CPU Tables:** IDT (Interrupt Descriptor Table), GDT (Global Descriptor Table), and LDT (Local Descriptor Table) configured.
- [x] **Privilege Rings:** Kernel Space (Ring 0) and User Space (Ring 3) separation.
- [x] **Terminal:** VT100 escape sequence support *(partially implemented)*.

### 🚧 To Do (Not Yet Implemented)
- [ ] **Process Management & Scheduler**
- [ ] **System Calls (Syscalls)**
- [ ] **Filesystem**
- [ ] **Networking Stack**
- [ ] **ELF Loader & Linker**
- [ ] **Kernel Modules**

---

## 🛠️ Dependencies & Requirements

Because the project spans two different languages depending on the branch you are looking at, the requirements differ slightly.

### Common Requirements
* **QEMU (x86):** Required to run and emulate the kernel.

### C Version (`main` branch)
* **Cross-Compiler:** A standard GCC cross-compiler toolchain for x86.
* **Make:** To handle the build system and compilation.

### Zig Version (`zig-kfs` branch)
* **Zig Compiler:** Version `>= 0.15.0` is strictly required. 
  *(Note: You do not need a separate cross-compiler for this branch, as Zig handles cross-compilation natively out of the box!)*

---

## ⚙️ Building and Running

### Nix flake file

If you have flake you can use that to get the proper version and all the depencies by doing:

```bash
# Clone the repository
git clone https://github.com/qbornet/kfs.git
cd kfs

# Nix development shell (main branch)
nix develop 

make
```

```bash
# Nix development shell (zig branch)
git checkout zig-kfs
nix develop

# Build the kernel.bin & iso.
zig build
```
each branch will have different version of the `flake.nix` file both of them will work as attended.

### For the Zig Branch (`zig-kfs`)

Make sure you have Zig `0.15.0` or newer installed, then run:

```bash
# Clone the repository and switch to the Zig branch
git clone https://github.com/qbornet/kfs.git
cd kfs
git checkout zig-kfs

# Build the kernel
zig build

# (Check build.zig for specific run targets, typically something like:)
zig build run
```

### For the C Branch (`main`)

Make sure your GCC cross-compiler is in your `$PATH`, then run:

```bash
# Clone the repository
git clone https://github.com/qbornet/kfs.git
cd kfs

# Compile the kernel
make

# Run in QEMU
make run
```
