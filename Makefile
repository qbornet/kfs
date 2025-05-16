CC = i686-elf-gcc
NASM = nasm
LD = i686-elf-ld
FORMAT = clang-format
TIDY = clang-tidy

CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -Werror -g3 -MMD \
         -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs
LDFLAGS = -T linker.ld -nostdlib


SRC_DIR = src
OBJ_DIR = obj
ASM_DIR = $(SRC_DIR)/asm
VGA_DIR = $(SRC_DIR)/vga
LIB_DIR = $(SRC_DIR)/lib
GDT_DIR = $(SRC_DIR)/gdt
PAGING_DIR = $(SRC_DIR)/paging
ISO_DIR = iso

C_SOURCES = $(wildcard $(SRC_DIR)/*.c) \
			$(wildcard $(VGA_DIR)/*.c) \
			$(wildcard $(LIB_DIR)/*.c) \
			$(wildcard $(GDT_DIR)/*.c) \
			$(wildcard $(PAGING_DIR)/*.c) 

ASM_SOURCES = $(wildcard $(ASM_DIR)/*.asm)

HEADERS = $(wildcard $(SRC_DIR)/*.h) \
		  $(wildcard $(VGA_DIR)/*.h) \
		  $(wildcard $(LIB_DIR)/*.h) \
		  $(wildcard $(GDT_DIR)/*.h) \
		  $(wildcard $(PAGING_DIR)/*.h)
		  


C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))
VGA_OBJECTS = $(patsubst $(VGA_DIR)/%.c, $(OBJ_DIR)/vga/%.o, $(filter $(VGA_DIR)/%.c, $(C_SOURCES)))
LIB_OBJECTS = $(patsubst $(LIB_DIR)/%.c, $(OBJ_DIR)/lib/%.o, $(filter $(LIB_DIR)/%.c, $(C_SOURCES)))
GDT_OBJECTS = $(patsubst $(GDT_DIR)/%.c, $(OBJ_DIR)/gdt/%.o, $(filter $(GDT_DIR)/%.c, $(C_SOURCES)))
PAGING_OBJECTS = $(patsubst $(PAGING_DIR)/%.c, $(OBJ_DIR)/paging/%.o, $(filter $(PAGING_DIR)/%.c, $(C_SOURCES)))
ASM_OBJECTS = $(patsubst $(ASM_DIR)/%.asm, $(OBJ_DIR)/asm/%.o, $(ASM_SOURCES))

OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS) $(VGA_OBJECTS) $(LIB_OBJECTS) $(GDT_OBJECTS) $(PAGING_OBJECTS)

GRUB_DIR = $(ISO_DIR)/boot/grub
ISO_FILE = nilbogos.iso

all: directories format tidy kernel.bin

format: $(C_SOURCES) $(HEADERS)
	@$(FORMAT) -i $(HEADERS) $(C_SOURCES)

tidy: $(C_SOURCES) $(HEADERS)
	@$(TIDY) --fix --fix-errors --fix-notes --config-file=$(PWD)/.clang-tidy \
		--quiet $(C_SOURCES) $(HEADERS) -- -std=gnu99

directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/asm
	mkdir -p $(OBJ_DIR)/vga
	mkdir -p $(OBJ_DIR)/lib
	mkdir -p $(OBJ_DIR)/gdt
	mkdir -p $(OBJ_DIR)/paging

kernel.bin: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -I$(LIB_DIR) -c $< -o $@

$(OBJ_DIR)/vga/%.o: $(VGA_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -I$(LIB_DIR) -c $< -o $@

$(OBJ_DIR)/asm/%.o: $(ASM_DIR)/%.asm
	$(NASM) -f elf32 $< -o $@


$(ISO_FILE):
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
	qemu-system-i386 -cdrom $(ISO_FILE)

run-terminal: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -nographic -enable-kvm -serial mon:stdio -display curses

debug: $(ISO_FILE)
	qemu-system-i386 -cdrom $(ISO_FILE) -nographic -enable-kvm -serial mon:stdio -s -S

clean:
	rm -rf $(OBJ_DIR) kernel.bin $(ISO_FILE) $(ISO_DIR)/boot/kernel.bin

re: clean all

-include $(OBJ_DIR)/*.d

.PHONY: all clean directories debug run re format tidy
