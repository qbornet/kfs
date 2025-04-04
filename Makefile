SOURCES_ASM := asm/boot.asm
ASM_NAMES := boot.bin


all: $(ASM_NAMES)

$(ASM_NAMES): $(SOURCES_ASM)
	nasm -f bin $(SOURCES_ASM) -o $@

run:
	qemu-system-i386 -fda boot.bin -nographic -serial mon:stdio

debug:
	qemu-system-i386 -fda boot.bin -nographic -serial mon:stdio -s -S

clean:
	rm -rf boot.bin

re: clean all

.PHONY: debug run re clean
