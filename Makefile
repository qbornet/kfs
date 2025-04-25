CC = i686-elf-gcc
NASM = nasm
LD = i686-elf-ld

CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -Werror -g3 -MMD \
         -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs
LDFLAGS = -T linker.ld -nostdlib


SRC_DIR = src
OBJ_DIR = obj
ASM_DIR = $(SRC_DIR)/asm
VGA_DIR = $(SRC_DIR)/vga
ISO_DIR = iso

C_SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(VGA_DIR)/*.c)
ASM_SOURCES = $(wildcard $(ASM_DIR)/*.asm)
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(VGA_DIR)/*.h)


C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))
VGA_OBJECTS = $(patsubst $(VGA_DIR)/%.c, $(OBJ_DIR)/vga/%.o, $(filter $(VGA_DIR)/%.c, $(C_SOURCES)))
ASM_OBJECTS = $(patsubst $(ASM_DIR)/%.asm, $(OBJ_DIR)/asm/%.o, $(ASM_SOURCES))

OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS) $(VGA_OBJECTS)

GRUB_DIR = $(ISO_DIR)/boot/grub
ISO_FILE = nilbogos.iso

all: directories kernel.bin

directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/asm
	mkdir -p $(OBJ_DIR)/vga

kernel.bin: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

$(OBJ_DIR)/vga/%.o: $(VGA_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

$(OBJ_DIR)/asm/%.o: $(ASM_DIR)/%.asm
	$(NASM) -f elf32 $< -o $@


iso: kernel.bin
	@cp -f kernel.bin $(ISO_DIR)/boot/
	@echo "Generating ISO image..."
	xorriso -as mkisofs \
		-o nilbogos.iso \
		-b boot/grub/i386-pc/eltorito.img \
		-c boot.catalog \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table iso

run: iso
	qemu-system-i386 -cdrom $(ISO_FILE) -enable-kvm

debug: iso
	qemu-system-i386 -cdrom $(ISO_FILE) -nographic -enable-kvm -serial mon:stdio -s -S

clean:
	rm -rf $(OBJ_DIR) kernel.bin $(ISO_FILE) $(ISO_DIR)/boot/kernel.bin

re: clean all

-include $(OBJ_DIR)/*.d

.PHONY: all clean directories debug run re
