# Conditional statement for nixos env or not.
CC := $(if $(NIXOS_DEV_ENV),i686-unknown-linux-gnu-gcc,i686-elf-gcc)
LD := $(if $(NIXOS_DEV_ENV),i686-unknown-linux-gnu-ld,i686-elf-ld)
OBJCOPY := $(if $(NIXOS_DEV_ENV),i686-unknown-linux-gnu-objcopy,i686-elf-objcopy)

NASM = nasm
TIDY = clang-tidy
FORMAT = clang-format

CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -Werror -g3 -O0 -MMD \
         -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs
LDFLAGS = -T linker.ld -z noexecstack -nostdlib

SRC_DIR = src
OBJ_DIR = obj
IDT_DIR = $(SRC_DIR)/idt
ASM_DIR = $(SRC_DIR)/asm
VGA_DIR = $(SRC_DIR)/vga
LIB_DIR = $(SRC_DIR)/lib
BOOT_DIR = $(SRC_DIR)/boot
PAGING_DIR = $(SRC_DIR)/paging
SERIAL_DIR = $(SRC_DIR)/serial
ISO_DIR = iso

C_SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(VGA_DIR)/*.c) $(wildcard $(LIB_DIR)/*.c) $(wildcard $(SERIAL_DIR)/*.c) $(wildcard $(PAGING_DIR)/*.c) $(wildcard $(BOOT_DIR)/*.c) $(wildcard $(IDT_DIR)/*.c)
ASM_SOURCES = $(wildcard $(ASM_DIR)/*.asm)
HEADERS = $(wildcard $(SRC_DIR)/**/*.h) $(wildcard $(VGA_DIR)/**/*.h) $(wildcard $(LIB_DIR)/**/*.h) $(wildcard $(SERIAL_DIR)/**/*.h) $(wildcard $(BOOT_DIR)/**/*.h)

C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))
IDT_OBJECTS = $(patsubst $(IDT_DIR)/%.c, $(OBJ_DIR)/idt/%.o, $(filter $(IDT_DIR)/%.c, $(C_SOURCES)))
VGA_OBJECTS = $(patsubst $(VGA_DIR)/%.c, $(OBJ_DIR)/vga/%.o, $(filter $(VGA_DIR)/%.c, $(C_SOURCES)))
LIB_OBJECTS = $(patsubst $(LIB_DIR)/%.c, $(OBJ_DIR)/lib/%.o, $(filter $(LIB_DIR)/%.c, $(C_SOURCES)))
BOOT_OBJECTS = $(patsubst $(BOOT_DIR)/%.c, $(OBJ_DIR)/boot/%.o, $(filter $(BOOT_DIR)/%.c, $(C_SOURCES)))
PAGING_OBJECTS = $(patsubst $(PAGING_DIR)/%.c, $(OBJ_DIR)/paging/%.o, $(filter $(PAGING_DIR)/%.c, $(C_SOURCES)))
SERIAL_OBJECTS = $(patsubst $(SERIAL_DIR)/%.c, $(OBJ_DIR)/serial/%.o, $(filter $(SERIAL_DIR)/%.c, $(C_SOURCES)))
ASM_OBJECTS = $(patsubst $(ASM_DIR)/%.asm, $(OBJ_DIR)/asm/%.o, $(ASM_SOURCES))

OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS) $(VGA_OBJECTS) $(LIB_OBJECTS) $(BOOT_OBJECTS) $(IDT_OBJECTS)
GRUB_DIR = $(ISO_DIR)/boot/grub
BIN_FILE = kernel.bin
SYM_FILE = kernel.sym
ISO_FILE = nilbogos.iso


all: directories format tidy $(BIN_FILE)

directories:
	@echo $(CC)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/asm
	mkdir -p $(OBJ_DIR)/idt
	mkdir -p $(OBJ_DIR)/vga
	mkdir -p $(OBJ_DIR)/lib
	mkdir -p $(OBJ_DIR)/boot
	mkdir -p $(OBJ_DIR)/serial
	mkdir -p $(OBJ_DIR)/paging

format: $(C_SOURCES) $(HEADERS)
	@$(FORMAT) -i $(HEADERS) $(C_SOURCES)

tidy: $(C_SOURCES) $(HEADERS)
	@$(TIDY) --fix --fix-errors --fix-notes --config-file=$(PWD)/.clang-tidy \
		--quiet $(C_SOURCES) $(HEADERS) -- -Isrc -std=gnu99

$(BIN_FILE): $(OBJECTS) 
	$(LD) $(LDFLAGS) -o $@ $^

$(SYM_FILE): $(BIN_FILE)
	$(OBJCOPY) --only-keep-debug $^ $@

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

$(OBJ_DIR)/asm/%.o: $(ASM_DIR)/%.asm
	$(NASM) -f elf32 -g -F dwarf $< -o $@


$(ISO_FILE): $(BIN_FILE)
	@cp -f kernel.bin $(ISO_DIR)/boot/
	@echo "Generating ISO image..."
	xorriso -as mkisofs \
		-o nilbogos.iso \
		-b boot/grub/i386-pc/eltorito.img \
		-c boot.catalog \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table iso

run: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -serial mon:stdio

run-terminal: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -nographic -serial mon:stdio

run-tlb-mode: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -serial mon:stdio -monitor unix:/tmp/qemu-monitor.sock,server,nowait

fetch-tlb:
	echo "info tlb" | socat - UNIX-CONNECT:/tmp/qemu-monitor.sock > tlb.dump


debug: $(ISO_FILE) $(SYM_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -nographic -serial mon:stdio -s -S -display curses

clean:
	rm -rf $(OBJ_DIR) $(BIN_FILE) $(SYM_FILE) $(ISO_FILE) $(ISO_DIR)/boot/kernel.bin

gdb:
	gdb -x "gdbscript" kernel.bin

re: clean all

-include $(OBJ_DIR)/*.d

.PHONY: all clean directories debug run re format tidy
