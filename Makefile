CC = clang
NASM = nasm
LD = ld

CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -Werror -MMD \
         -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -m32
LDFLAGS = -T linker.ld -nostdlib -m32

SRC_DIR = src
OBJ_DIR = obj
ISO_DIR = iso
GRUB_DIR = $(ISO_DIR)/boot/grub
ISO_FILE = nilbogos.iso

all: kernel.bin

kernel.bin: $(OBJ_DIR)/boot.o $(OBJ_DIR)/kernel.o
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJ_DIR)/boot.o $(OBJ_DIR)/kernel.o

$(OBJ_DIR)/boot.o: $(SRC_DIR)/boot.asm | $(OBJ_DIR)
	$(NASM) -f elf32 $< -o $@

$(OBJ_DIR)/kernel.o: $(SRC_DIR)/kernel.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

iso: kernel.bin
	@echo "Creating ISO directory structure..."
	@mkdir -p $(ISO_DIR)/boot
	@mkdir -p $(GRUB_DIR)
	@cp kernel.bin $(ISO_DIR)/boot/
	@echo "Generating ISO image..."
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)

run: iso
	qemu-system-i386 -cdrom $(ISO_FILE) -enable-kvm -nographic -serial mon:stdio

debug:
	qemu-system-i386 -cdrom $(ISO_FILE) -nographic -serial mon:stdio -s -S

clean:
	rm -rf $(OBJ_DIR) kernel.bin $(ISO_FILE) $(ISO_DIR)/boot/kernel.bin

-include $(OBJ_DIR)/*.d
